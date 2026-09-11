// b28d-solo-W8_CU_G: DIFF/N 封顶 — ORIG 0x0868f956 三失败出口各落独立 nop 岛
//（+0x83 nop;jmp / +0x86 nop;jmp / +0x89 nop，标签序=守卫序）⇒ P223 负条件
// goto 连续尾标签形复现三岛（探针同 CCharacInformNotice::handleInformNoticeFlag
// 同族定案 0x080e4806）。残差 N：①ORIG 读值/和值各多一条寄存器中转
//（`movzbl→%edx; mov %edx,%ebx` / `lea→%edx; mov %edx,%ecx`），4.4 恒折单段
//（N18a 正向，字段/RHS 均已异型仍不可造）；②第三守卫 ORIG `test %al,%al; je`
// vs 4.4 `xor $0x1; test; jne`（P168a①/P292 bool 归一，无绑定不可造）。
// 55/54，语义等价。
void CUser::handleInformNoticeFlag(unsigned int bitIndex)
{
    // P133：ORIG 三守卫各自 jne/je 直跳统一失败出口（bit>7 → byte 计算 →
    // byte>0xf → check），独立 if 不可合并短路。
    unsigned int bit = bitIndex & 7;
    if (bit > 7)
        goto L1;
    {
        unsigned int byte = bitIndex >> 3;
        if (byte > 0xf)
            goto L2;
        // b28d-W8: ORIG 0x0868f996 `call; test %al,%al; je <岛>`（bool callee 直判、
        // 无 xor）；4.4 对 `!call()`/`call()==0` 恒产 `xor $1; test; jne`（+1 条）⇒
        // P168a③b 跨版本 bool ABI 封 N；正极性 if 包裹实测 51（少 4 条，L1/L2 岛被
        // jump-thread），if/else+goto 形同（b28d-W8b 复测 51）已回退。
        if (checkInformNoticeFlag(bit, byte) == 0)
            goto L3;
        // ORIG lea (%ebx,%edx,1) 加法形 ⇒ 源码 +（位不重叠与 | 等价）。
        m_informNoticeFlags[byte] = m_informNoticeFlags[byte] + (1 << bit);
        enableSaveInformNoticeFlag();
        goto END;
    }
L1:;
L2:;
L3:;
END:;
}