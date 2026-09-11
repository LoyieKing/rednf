// ORIG 0x8443d2c：GetOutBuffer<SIG_MILEAGE_STRUCT>（+0=m_id/+4=charac_no/
// +8=miles）→ GetDBHandle(1,0) 失败 cMyTrace(0xa7ae) 返 0 → seLect miles
// exec 失败 cMyTrace(0xa7b6) 返 0；无行 inSert 新行并 miles=0；有行
// fetch/get_int(0) 失败各 cMyTrace(0xa7cb/0xa7d2) 返 0；最后
// Acquire("DBThread.cpp",0xa7d8) → CStreamGuard(stream,true) → <<userIdx →
// <<slot → put_binary(sig,0xc) → put(DISPATCH_Q,guard) 返其结果。
bool DB_LoadMileage::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIndex = 1;  // ORIG _ZZN14DB_LoadMileage8dispatch…E11handleIndex
    SIG_MILEAGE_STRUCT* sig = stream->GetOutBuffer<SIG_MILEAGE_STRUCT>();
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIndex, E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa7ae, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 1);
        return 0;
    }
    db->set_query("seLect miles from member_miles where m_id = %u",
                  ((DB_MileageView*)sig)->m_mId);
    bool r = db->exec(true);
    if (!r)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa7b6, 5)(
            "[ERROR] Exec [m_id:%u][charac_no:%u]",
            ((DB_MileageView*)sig)->m_mId, ((DB_MileageView*)sig)->m_characNo);
        return 0;
    }
    // ORIG local_10：get_n_rows 结果先落 -0xc int 槽再判 0（缺此槽则整链
    // sig/db/trace 上移一格）。
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        db->set_query(
            "inSert into member_miles (m_id, miles, daily_miles) values (%u, 0, 0)",
            ((DB_MileageView*)sig)->m_mId);
        db->exec(true);
        ((DB_MileageView*)sig)->m_miles = 0;
    }
    else
    {
        r = db->fetch();
        if (!r)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa7cb, 5)(
                "[ERROR] Fetch [m_id:%u][charac_no:%u]",
                ((DB_MileageView*)sig)->m_mId, ((DB_MileageView*)sig)->m_characNo);
            return 0;
        }
        r = db->get_int(0, ((DB_MileageView*)sig)->m_miles);
        if (!r)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa7d2, 5)(
                "[ERROR] Mileage [m_id:%u][charac_no:%u]",
                ((DB_MileageView*)sig)->m_mId, ((DB_MileageView*)sig)->m_characNo);
            return 0;
        }
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa7d8), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0xc);
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}