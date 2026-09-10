std::vector<WongWork::SUserPremiumInfo> WongWork::CUserPremium::GetPremiumInfoList(
    int type) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE（P157/P183 去缓存 + P223 双岛）]
    // ORIG 0x086ade9c：sret=0x8(%ebp)、
    // this=0xc(%ebp)（m_entries@+0x00）；ORIG 每处字段读都重发完整 `i*0x14 + this`
    // 链（shl/add/shl + `mov (%eax,%ecx,1)`），**无** entries 指针槽与元素引用槽 ⇒
    // 源码必须逐字段下标直写（禁 `entries`/`entry` 缓存，旧版多 9 条槽往返）。
    // ORIG T 0x086ade9c (size 0xb6)：遍历 entries[1..0x69]（stride 0x14），
    // m_type != 0 且 filter==0x270f 或等于 m_type 时 push_back。
    std::vector<SUserPremiumInfo> list;
    for (int i = 1; i <= 0x69; ++i)
    {
        // P223 多岛形：ORIG 两失败出口落**不同** nop 岛（0x86adf15 / 0x86adf18，
        // 各带/共享 jmp 到循环尾 0x86adf19）⇒ 两条独立否定 goto + 标签串，
        // 标签序 = 守卫序（合并 && 会被 4.4 jump-thread 成 0 岛）。
        if (m_entries[i].m_type == 0)
            goto fail1;
        if (type != 0x270f && m_entries[i].m_type != type)
            goto fail2;
        list.push_back(m_entries[i]);
fail1:;
fail2:;
    }
    return list;
