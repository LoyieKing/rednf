void get_additional_item(std::vector<stItemSelectRate> const& table, int randValue,
                         stItemSelectRate& out, int grade)
{
    // [b28d-solo-ExpertJobLink4] P13/P28：ORIG begin→it(-0xc)、end→e(-0x10)
    // 两具名迭代器（循环初始化区一次求值）；单语句 `it != table.end()` 让 4.4
    // 把 end 临时放深槽致 it/e 槽位互换。
    for (std::vector<stItemSelectRate>::const_iterator it = table.begin(),
             e = table.end();
         it != e; ++it)
    {
        out = *it;
        // [b28d-solo-ExpertJobLink4] P223 多岛细则：ORIG 两个失败出口各落独立
        // nop 中继岛（0x896d733 `nop;jmp` / 0x896d736 `nop`）+ body 尾 `jmp`；
        // 唯一复现形 = 两条独立失败 goto + 标签串（标签序=守卫序）。
        if (out.m_rateMin > grade)
            goto skip_min;
        if (out.m_rateMax < grade)
            goto skip_max;
            // ORIG 0x896d723 `cmp randValue,%eax(m_rate); jge <ret>`：m_rate >=
            // randValue 才返回（原 `randValue >= m_rate` 等号方向写反 = 真语义 bug）。
            if (out.m_rate >= randValue)
                return;
            randValue -= out.m_rate;
    skip_min:;
    skip_max:;
    }
