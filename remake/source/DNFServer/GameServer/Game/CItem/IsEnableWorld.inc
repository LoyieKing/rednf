bool CItem::IsEnableWorld(ENUM_WORLD_TYPE type) const
{
    if (m_setd4.find((int)type) != m_setd4.end())
    {
        return true;
    }
    return false;
