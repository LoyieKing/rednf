bool CParty::checkBossRoom()
{   // ORIG W 0x0822d8f6：al 透传 check_end_point 返回值（check_allmember_loading
    // 以 && 消费，原 void 声明与 ORIG 不符）
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->check_end_point();
}