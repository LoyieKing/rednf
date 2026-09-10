bool DB_EventUpdateGrowthWeaponCharac::dispatch(int userIdx, int slot,
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    unsigned int characNo = 0;
    // [b28d-solo] P167：ORIG +0x6d 第三个读取是 operator>>(int&)（差集
    // -1 rsERi/+1 rsERj 实证）；short 无清零（ORIG 仅三条 movl $0）。
    int growthweaponId = 0;
    short serverGroup;
    *stream >> m_id;
    *stream >> characNo;
    *stream >> growthweaponId;
    *stream >> serverGroup;
    db->set_query(
        "upDate event_1207_growthweapon set occ_date = now(), charac_no = %d, growthweapon_id = %d, server_id = %d where m_id = %u",
        characNo, growthweaponId, (int)serverGroup, m_id);
    if (!db->exec(true))
        return 0;
    return 1;
