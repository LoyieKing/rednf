void RwSpinLock::enterWriteToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        // [b28d-41 判词·N3] DIFF：唯一残差 = ORIG CAS 后 `cmp;jne/mov $1;jmp/mov $0;
        // test %al,%al` 的 4.1 寄存器 select 物化（P56 分支返回形）vs 4.4 `sete %al;
        // test`；帧/跳转布局与其余 28 条已全等。b28c-lane25 探针 /tmp/l25probe/rw.cpp
        // fA/fB/fC 三形全败 + 本轮复核（N3 choose-without-store 负例，非 O0 可复现；
        // 无类型旋钮可试，比较两侧均 long）。
        if (ostate == 0 || ostate == 0x40000000)
        {
            if (__sync_val_compare_and_swap(value, ostate, 0x80000000) == ostate)
                break;
        }
        if ((ostate & 0x40000000) == 0)
            __sync_val_compare_and_swap(value, ostate, ostate | 0x40000000);
        YieldProcessor();
    }
