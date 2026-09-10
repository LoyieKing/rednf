bool CEquipItem::IsAvatarOptionValid(char idx) const
{
    if (idx < 0)
    {
        return 0;
    }
    if ((signed char)m_avatarAbility.size() <= idx)
    {
        return 0;
    }
    return 1;
