// ============================================================================
// DB_UpdateFatigueAttendance::dispatch —— ORIG 0x084444ec（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → map<unsigned char,short> 构造 → 解包 mId(uint)/
// count(int) → 循环 count 次：解包 occDay(uchar)/fatigue(short) 后 insert 入 map
// → begin/end 迭代遍历：it->second > 0 时 set_query("upDate %s set fatigue =
// fatigue + %d where occ_day = %d and m_id = %s", tableName, it->second,
// it->first, NumberToString(mId,0))（表名 = std::string(get_db_table_name(0x56))
// 经 toTString）→ exec 失败或 affectedRowCount==0 → inSert into %s(occ_day,m_id,
// fatigue) values(%d,%s,%d)（tableName, occDay, mIdStr, fatigue），insert exec 返
// 回值丢弃；循环完返 1（ORIG 恒 1）。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 084444ec
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
        // [J4S3-12 P171 反向] ORIG 0x08444559 之后直接 lea 取址解包，无 movb/movw
        // 预清（裸声明只占槽不发指令）⇒ 删初值。
        unsigned char occDay;   // -0x59
        short fatigue;          // -0x5c
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
}