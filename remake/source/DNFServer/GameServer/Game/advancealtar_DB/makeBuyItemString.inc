void DB_Update::makeBuyItemString(std::string& out, const _BuyItem* arr, int count) // [objdump 0x813f53c]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f619 fmt="%d%s%d%s%d%s%d%s"，"," ×3 + "|"（实证装载序列
        // 0x0813f5d5..0x0813f615；双 short 为 movzwl+movswl 符号扩展，与
        // static_cast<int> 同形）。同 makeRewardString 配方。
        // [b28d-26] ORIG 0x0813f644/646 `jmp +0x10a` + `nop@0x109`：P223 双源 goto
        //   尾标签串复现（条件取反形，同 makeRewardString）。
        if (arr[i].m_field4 == 0 && arr[i].m_short8 == 0)
            goto L_skip;
        std::sprintf(buf, "%d%s%d%s%d%s%d%s", arr[i].m_field0, ",",
                     arr[i].m_field4, ",",
                     static_cast<int>(arr[i].m_short8), ",",
                     static_cast<int>(arr[i].m_short10), "|");
        out += buf;
        goto L_inc;
L_skip:;
L_inc:;
    }
}