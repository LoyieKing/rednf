int CStackableItem::get_1st_material_count(unsigned int itemIdx)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_limitCube.m_materials.begin();
         it != m_limitCube.m_materials.end(); it++)
    {
        if ((unsigned int)it->first == itemIdx)
        {
            return it->second;
        }
    }
    return -1;
}