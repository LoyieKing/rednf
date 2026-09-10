CItem* NeedMaterialDataManager::getNeedMaterial(const CItem* item) const
{
    if (item == 0)
    {
        return (CItem*)&m_default;
    }
    int index = item->get_index();
    std::map<int, std::pair<int, int> >::const_iterator it = m_map.find(index);
    if (it != m_map.end())
    {
        return (CItem*)&it->second;   // ORIG 用 operator->（pt）
    }
    return (CItem*)&m_default;
