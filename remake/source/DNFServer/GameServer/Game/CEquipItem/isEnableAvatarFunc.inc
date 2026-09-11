bool CEquipItem::isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const
{
    return (m_field18c & (int)filter) == (int)filter;
}