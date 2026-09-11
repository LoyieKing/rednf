// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_updatePurchaseCount @ 0x0844b680
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→ set_query(
//   " upDate event_purchase_cnt set purchase_cnt=%d where m_id = %s ",
//   count, NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)
//   （失败 throw "exec"）。catch(const char* e) → trace(0xbc22)
//   "update purchase count (error:%s)"。void 返回。
// ============================================================================
void DB_UpdateCeraShopPurchaseCount::_updatePurchaseCount(memberIdentificationNumber_t mId,
                                                          unsigned short count)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        if (!db->set_query(
                " upDate event_purchase_cnt set purchase_cnt=%d where m_id = %s ",
                count, NumberToString(mId, 0)))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
    }
    catch (const char* e)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbc22, 5);
        trace("[PURCAHSE BONUS] update purchase count (error:%s)", e);
    }
}