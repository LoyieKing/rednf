// ORIG T 0x085bea4a（无报告，objdump）：al 直接透传 check_grid_clear。
bool CParty::CheckMapClear()
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->check_grid_clear();
}