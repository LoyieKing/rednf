bool CItem::IsEnableWorld(const CItem* other) const
{
    for (std::set<int>::const_iterator it = other->m_setd4.begin();
         it != other->m_setd4.end(); ++it)
    {
        if (!IsEnableWorld((ENUM_WORLD_TYPE)*it))
        {
            return false;
        }
    }
    return true;
