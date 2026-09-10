bool CParty::CheckMapClear()
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->check_grid_clear();
