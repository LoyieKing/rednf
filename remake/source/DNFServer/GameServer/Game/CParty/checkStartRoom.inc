bool CParty::checkStartRoom()
{   // ORIG 0x0826525e：al 直接透传 check_start_point 返回值
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->check_start_point();
}