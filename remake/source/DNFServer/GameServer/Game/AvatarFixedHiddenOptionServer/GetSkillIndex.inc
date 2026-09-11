int AvatarFixedHiddenOptionServer::GetSkillIndex(ENUM_RARITY rarity, int randIdx)
{
    // ORIG 0x0817e23e：无 empty() 前置检查，直接 back()（GetRandomInput 保证非空进入）。
    // [b28d-56 判词·AE] P183/P157 去引用缓存：ORIG 每处访问都重发完整
    // `mov rarity,%edx; mov %edx,%eax; add %eax,%eax; add %edx,%eax; shl $2;
    // add $0x50; add this` 链（back/end/begin/begin 共 4 处），无 rates 引用槽。
    if (!(randIdx < m_cumulativeRate[rarity].back() && 0 <= randIdx))
        return -1;
    std::vector<int>::iterator up =
        std::upper_bound(m_cumulativeRate[rarity].begin(),
                         m_cumulativeRate[rarity].end(), randIdx);
    return (int)(up - m_cumulativeRate[rarity].begin()) + 1;
}