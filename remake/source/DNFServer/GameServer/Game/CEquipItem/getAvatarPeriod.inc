int CEquipItem::getAvatarPeriod(unsigned char a) const
{
    // ORIG 0x8151120：result 默认 = getUsablePeriod(); 若 a < avatarSelect.size()，
    // 则 result = avatarSelect[a].m_field0。
    // ORIG 经 getAvatarTypeSelect() 取视图且比较为有符号（cmp+setl）→ 同型复刻。
    const std::vector<stAvatarTypeSelect_t>* pSel =
        (const std::vector<stAvatarTypeSelect_t>*)getAvatarTypeSelect();
    int result = getUsablePeriod();
    if ((int)a < (int)pSel->size())
        result = (*pSel)[a].m_field0;
    return result;
