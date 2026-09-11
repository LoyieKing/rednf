bool ServerParameterScript::CheckUnusableItem(ENUM_EQUIPMENTTYPE equipType,
                                              unsigned long itemIdx) const
{
    // b28d-solo-ServerParameterScript: 真语义 bug 修正 + 形态修（P293 具名迭代器）
    // ORIG 0x08568bbe 逐段实证：① m_unusableItemFC 命中 → return false；
    // ② m_map114（[usable items] **白名单**）未列出该 equipType → 尾块 return true；
    // ③ 列出且集合**命中** itemIdx → 跳尾块 return true（ORIG `call eq; test; je
    // 8568cd8`：eq 假=sit!=end 才跳 true）；集合未命中 → mov $0 return false。
    // 旧源码 `if (it!=end && set.find!=set.end()) return false;` 把白名单判成黑名单，
    // 命中时返回 false 与 ORIG 相反 ⇒ 真语义 bug。单表达式 `find()!=end()` 在 4.4 恒
    // end 先/find 后（本次实测），故三处均改具名迭代器；尾块只留一处 return true。
    std::set<unsigned long>::const_iterator unusableIt =
        m_unusableItemFC.find(itemIdx);
    if (unusableIt != m_unusableItemFC.end())
    {
        return false;
    }
    std::map<ENUM_EQUIPMENTTYPE, std::set<unsigned long> >::const_iterator it =
        m_map114.find(equipType);
    if (it != m_map114.end())
    {
        std::set<unsigned long>::const_iterator sit = it->second.find(itemIdx);
        if (sit == it->second.end())
        {
            return false;
        }
    }
    return true;
}