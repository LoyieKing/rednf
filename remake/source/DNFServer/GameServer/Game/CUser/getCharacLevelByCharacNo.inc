unsigned short CUser::getCharacLevelByCharacNo(int characNo) const  // ORIG caller 0x84d205b mov %ax 截断+cmpw 实证 16 位返回（Main 落头 2026-09-05）
{
    // ORIG 0x0868e5xx：begin→-0x10、end→-0x14 prologue 双缓存（技巧 25）；
    // return 无显式 cast（ORIG 尾单条 movzwl 直返，(unsigned short) 强转反产两段）。
    int idx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itend = m_characList.end();
    for (; it != itend; ++idx, ++it)   // ORIG 0x84ecc4f ++idx 先于 ppEv
    {
        if (it->m_characNo == characNo)
            // 函数返回类型 ushort（Main 落头）后裸 return：4.4 隐式 HImode 截断
            // 单条 movzwl 0x27(%eax)（ORIG 0x84ecc49 同形）；显式 cast 反产
            // movzwl %ax 二段展宽（probe 三试）——本行为定稿。
            return it->m_level;
    }
    return -1;
}