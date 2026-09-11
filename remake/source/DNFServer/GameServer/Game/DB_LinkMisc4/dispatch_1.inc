// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::dispatch @ 0x084496f4
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece8dispatchEiiP6Stream
// GetDBHandle(DB_HANDLE_9, 0) → >>m_id(u32) → >>timepiece(short) →
// >>infinityweapon_id(int)；id==0 时 UPDATE 不带 reward_occ_date，
// 否则带 reward_occ_date = now()；exec 成功返回 true。
// ============================================================================
bool DB_EventUpdateGrowthWeaponTimepiece::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    short timepiece = 0;
    int infinityweapon_id = 0;
    *stream >> m_id;
    *stream >> timepiece;
    *stream >> infinityweapon_id;
    if (infinityweapon_id != 0)
    {
        db->set_query("upDate event_1207_growthweapon set  reward_occ_date = now(), "
                      "timepiece = %d, infinityweapon_id = %d  where m_id = %u",
                      (int)timepiece, infinityweapon_id, m_id);
    }
    else
    {
        db->set_query("upDate event_1207_growthweapon set  timepiece = %d, "
                      "infinityweapon_id = %d  where m_id = %u",
                      (int)timepiece, infinityweapon_id, m_id);
    }
    if (!db->exec(true))
        return false;
    return true;
}