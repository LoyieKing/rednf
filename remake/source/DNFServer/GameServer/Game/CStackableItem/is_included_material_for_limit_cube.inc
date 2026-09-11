bool CStackableItem::is_included_material_for_limit_cube(unsigned int itemIdx)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_limitCube.m_materials.begin();
         it != m_limitCube.m_materials.end(); it++)
    {
        if ((unsigned int)it->first == itemIdx)
        {
            return 1;
        }
    }
    return 0;
}