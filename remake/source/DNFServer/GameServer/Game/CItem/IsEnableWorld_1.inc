// ORIG 0x822c8ea：subset 检查。遍历 other->m_setd4，若任一世界类型本对象
// 未启用则 false；全部启用（含空集）则 true。
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
}