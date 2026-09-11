void DB_Update::makeRewardString(std::string& out, const _Reward* arr, int count) // [objdump 0x813f66a]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f6e2 fmt="%d%s%d%s"(@0x8b6db4d)，逗号/竖线为独立 rodata 串常量
        // （","@0x8b6d831 / "|"@0x8b6d82f，readelf 实证）→ 6 实参槽 outgoing 区
        // 48B，帧 0xb8；先前 "%d,%d|" 2 参形态使帧缩为 0xa8（Main 指令之缓冲
        // 16B 维度差的真实来源 = sprintf 实参集，非 buf 本体：buf 基址 -0x8c
        // 两侧本就一致）。
        if (arr[i].m_field0 <= 0)
            goto L_skip;
        std::sprintf(buf, "%d%s%d%s", arr[i].m_field0, ",",
                     static_cast<int>(arr[i].m_byte4), "|");
        out += buf;
        goto L_inc;
L_skip:;
L_inc:;
        // [b28d-26] AE。旧「残余 jmp+nop 空块 N 类豁免」判词作废（P223 正例）：
        //   ORIG 0x0813f70d/0f `jmp +0xa6` + `nop@0xa5` 由双源 goto 尾标签串复现
        //   （`if(m<=0) goto L_skip; body; goto L_inc; L_skip:; L_inc:;`）。
    }
}