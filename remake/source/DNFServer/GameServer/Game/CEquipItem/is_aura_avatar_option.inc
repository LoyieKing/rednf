bool CEquipItem::is_aura_avatar_option(char option)
{
    if ((unsigned char)m_auraOptionType == (char)option)
    {
        return 1;
    }
    return 0;
}