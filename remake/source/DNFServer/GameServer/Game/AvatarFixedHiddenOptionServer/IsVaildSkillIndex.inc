bool AvatarFixedHiddenOptionServer::IsVaildSkillIndex(ENUM_RARITY rarity,
                                                      int idx)
{
    if (rarity >= 6)
        return false;
    int size = (int)m_abilities[rarity].size();
    // [b28c-16] P133：ORIG 两独立条件（cmpl $0;jne 续 / cmp;jge 落零），
    // 合并 `||` 4.4 产 je 单出口反形。
    if (size == 0)
        return false;
    if (idx < size)
        return true;
    return false;
}