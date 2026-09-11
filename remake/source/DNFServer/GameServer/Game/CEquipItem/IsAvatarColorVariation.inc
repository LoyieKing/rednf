unsigned char CEquipItem::IsAvatarColorVariation(int idx) const
{
    if (idx < 0 || idx > 1)
    {
        return 0;
    }
    return m_avatarColor[idx];
}