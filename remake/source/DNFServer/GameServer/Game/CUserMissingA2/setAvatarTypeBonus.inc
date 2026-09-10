void CUser::setAvatarTypeBonus(char bonus)
{
    if (bonus < 0 || 3 < bonus)
        return;

    if (getCurCharacR() == 0)
        return;

    int idx = m_mercenaryMgr.FindIndexByCharacNo(
        static_cast<unsigned int>(getCurCharacR()->m_characNo));
    if (idx == -1)
        return;
    if (!m_mercenaryMgr.CheckCharacIndex(static_cast<unsigned char>(idx)))
        return;

    m_mercenaryMgr.m_characs[idx].m_avatarBonus =
        static_cast<unsigned char>(bonus);
