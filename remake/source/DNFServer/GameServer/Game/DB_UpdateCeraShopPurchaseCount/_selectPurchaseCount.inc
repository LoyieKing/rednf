// ============================================================================
// DB_UpdateCeraShopPurchaseCount::_selectPurchaseCount @ 0x0844b450
// 逐指令转写：GetDBHandle(9,0)（null→throw "handle_null"）→
//   set_query(" seLect purchase_cnt from event_purchase_cnt where m_id = %s ",
//   NumberToString(mId,0))（失败 throw "set_query"）→ exec(true)（失败 throw
//   "exec"）→ get_n_rows()==1 时 fetch()（失败 throw "fetch"）+
//   get_ushort(0,out)（失败 throw "get_ushort"）；否则 out=0；返 true。
//   catch(const char* e) → trace(0xbc0b) "select purchase count (error:%s)" → false。
// ============================================================================
bool DB_UpdateCeraShopPurchaseCount::_selectPurchaseCount(memberIdentificationNumber_t mId,
                                                          unsigned short& out)
{
    try
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        if (db == 0)
            throw "handle_null";
        if (!db->set_query(
                " seLect purchase_cnt from event_purchase_cnt where m_id = %s ",
                NumberToString(mId, 0)))
            throw "set_query";
        if (!db->exec(true))
            throw "exec";
        if (db->get_n_rows() == 1)
        {
            if (!db->fetch())
                throw "fetch";
            if (!db->get_ushort(0, out))
                throw "get_ushort";
        }
        else
        {
            out = 0;
        }
        return true;
    }
    catch (const char* e)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbc0b, 5);
        trace("[PURCAHSE BONUS] select purchase count (error:%s)", e);
        return false;
    }
}