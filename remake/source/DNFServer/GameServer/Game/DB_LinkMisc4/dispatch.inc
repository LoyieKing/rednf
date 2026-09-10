bool DB_EventGrowthWeaponReset::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    db->set_query("deLete from event_1207_growthweapon where m_id = %u", m_id);
    if (!db->exec(true))
        return false;
    return true;
