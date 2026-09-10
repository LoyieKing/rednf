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
