bool DB_UpdateFatigueAttendance::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;    // -0x30
    int count = 0;           // -0x34
    std::map<unsigned char, short> fatigueMap;   // -0x4c（ORIG map C1）
    *stream >> mId;
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        unsigned char occDay = 0;   // -0x59
        short fatigue = 0;          // -0x5c
        *stream >> occDay;
        *stream >> fatigue;
        // ORIG：显式 pair 构造（_ZNSt4pairIKhsEC1IRhRsEEOT_OT0_，非 make_pair）
        fatigueMap.insert(
            std::pair<const unsigned char, short>(occDay, fatigue));
    }
    CDataManager* dm = G_CDataManager();
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(dm->get_event_script_mng());
    // ORIG 0x08444616：get_db_table_name(int) 返回 std::string&（_ZNSsC1ERKSs 拷贝构造）。
    std::string tableStr(sm->get_db_table_name(0x56));
    // [b28d-solo] P167：ORIG 两处 set_query 各自 toTString(tableStr)（无 CSE，
    // 差集 -1 toTString）⇒ 禁具名 tableName 缓存，逐处内联。
    for (std::map<unsigned char, short>::iterator it = fatigueMap.begin();
         it != fatigueMap.end(); ++it)
    {
        if (it->second > 0)
        {
            db->set_query(
                "upDate %s set fatigue = fatigue + %d where occ_day = %d and m_id = %s",
                toTString(tableStr), (int)it->second, (unsigned int)it->first,
                NumberToString(mId, 0));
            if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
            {
                db->set_query(
                    "inSert into %s(occ_day,m_id,fatigue) values(%d,%s,%d)",
                    toTString(tableStr), (unsigned int)it->first,
                    NumberToString(mId, 0), (int)it->second);
                db->exec(true);   // ORIG insert exec 返回值丢弃
            }
        }
    }
    return 1;
