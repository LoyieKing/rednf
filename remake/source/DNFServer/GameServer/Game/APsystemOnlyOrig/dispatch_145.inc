// ============================================================================
// DB_UpdateAddItemCountValue::dispatch —— ORIG 0x0843ed96（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → 解包 mId(uint)/itemCnt(uint) →
// G_CDataManager()->get_event_script_mng() → get_event_entity(0x193)，空返 0；
// 表名 = toTString(*(std::string*)entity->get_db_table_name())（vtable+0x20 虚调用）。
// upDate %s set item_cnt=item_cnt+%d where m_id=%s（table, itemCnt,
// NumberToString(mId,0)）；exec 失败返 0；affectedRowCount==0 → inSert into
// %s(m_id, item_cnt) values(%s, %d)（table, mIdStr, itemCnt），insert exec 失败
// 返 0；全绿返 1。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（112=112）。
bool DB_UpdateAddItemCountValue::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;     // -0x18
    unsigned int itemCnt = 0; // -0x1c
    *stream >> mId;
    *stream >> itemCnt;
    // ORIG：G_CDataManager() 结果不落槽（链式直传 get_event_script_mng）。
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* entity = sm->get_event_entity(0x193);
    if (entity == 0)
        return 0;
    // ORIG：表名不缓存（UPDATE/INSERT 各自 vtable+0x20 虚调用 + toTString，
    // 无 CSE）；mId 串每处 NumberToString 即时求值（ebx/esi callee-saved 缓存）。
    db->set_query("upDate %s set item_cnt=item_cnt+%d where m_id=%s",
                  toTString(entity->get_db_table_name()),
                  itemCnt, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into %s(m_id, item_cnt) values(%s, %d)",
                      toTString(entity->get_db_table_name()),
                      NumberToString(mId, 0), itemCnt);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}