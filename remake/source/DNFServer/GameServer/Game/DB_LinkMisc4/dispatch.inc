// ============================================================================
// DB_EventGrowthWeaponReset::dispatch @ 0x084498fe
// mangled _ZN25DB_EventGrowthWeaponReset8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_9, 0) → Stream>>m_id(u32) →
// set_query("deLete from event_1207_growthweapon where m_id = %u", m_id) →
// exec(true) 成功返回 true。
// ============================================================================
bool DB_EventGrowthWeaponReset::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    db->set_query("deLete from event_1207_growthweapon where m_id = %u", m_id);
    if (!db->exec(true))
        return false;
    return true;
}