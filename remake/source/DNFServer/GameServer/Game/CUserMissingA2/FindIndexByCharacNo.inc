unsigned int UserMercenaryInfoMgr::FindIndexByCharacNo(unsigned int characNo) const
{
    size_t idx = 0;
    while (m_characs.size() > idx)
    {
        if (static_cast<unsigned int>(m_characs[idx].m_characNo) == characNo)
            return static_cast<unsigned int>(idx);
        ++idx;
    }
    return 0xffffffffU;
