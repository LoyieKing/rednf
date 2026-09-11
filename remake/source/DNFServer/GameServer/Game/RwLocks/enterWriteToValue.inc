void RwSpinLock::enterWriteToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        // [P2-S4 2026-09-11 已翻] 原嵌套 `if (ostate==0||ostate==0x40000000) { if (CAS==ostate)
        //   break; }` 使 4.4 在条件上下文发 `sete %al; test`（33 条，DIFF gap=+3）。
        // 合并为**单条短路 && 条件**后 4.4 发射 ORIG 的物化形（37 条，verdict=AE gap=-1）：
        //   ORIG 0x08ad36ed `jne 0x8ad36f6`（CAS 不等 → mov $0,%eax）/
        //   0x08ad36ef `mov $1,%eax; jmp 0x8ad36fb` / 0x08ad36fb `test %al,%al; jne`
        //   0x08ad3726（return）。即 ORIG 源形 = 单条复合条件（4.1 对 &&/|| 结果做 0/1
        //   常量物化），非嵌套 if + 内层 break。
        if ((ostate == 0 || ostate == 0x40000000) &&
            __sync_val_compare_and_swap(value, ostate, 0x80000000) == ostate)
            break;
        if ((ostate & 0x40000000) == 0)
            __sync_val_compare_and_swap(value, ostate, ostate | 0x40000000);
        YieldProcessor();
    }
}