// ============================================================================
// DB_EventSaveGrowthWeapon::dispatch —— ORIG 0x084492fc
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) >>serverGroup(short) → inSert
// event_1207_growthweapon(m_id, server_id, occ_date)。exec 失败 →
// logFormat 0xb8e6 → 返 0；成功返 1。
// ============================================================================
bool DB_EventSaveGrowthWeapon::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    short serverGroup = 0;
    *stream >> m_id;
    *stream >> serverGroup;
    db->set_query(
        "inSert into event_1207_growthweapon(m_id, server_id, occ_date) values(%s, %d, now())",
        NumberToString(m_id, 0), (int)serverGroup);
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb8e6, "DB_EventSaveGrowthWeapon Query Error : m_id(%d)", m_id);
        return 0;
    }
    return 1;
}