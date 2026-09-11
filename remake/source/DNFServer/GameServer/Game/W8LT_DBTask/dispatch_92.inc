// ============================================================================
// DB_LoadPrivateStore::dispatch —— ORIG 0x0841c08c（报告 dispatch.md 全文）
// ============================================================================
// DBDispatcher::dispatch 前置失败返 0 → *stream >> characNo(int) →
// GetDBHandle(3,0) → seLect store, use_doll from store where charac_no=%d；
// exec 失败 cMyTrace(0x4f8b,5) 返 0。Acquire("DBThread.cpp",0x4f90) →
// guard << userIdx << slot → GetInBuffer<SIG_PRIVATE_STORE> → memset 0x434
// → m_characNo=characNo、m_field04=0；get_n_rows()!=0 时：
//   fetch 失败 cMyTrace(0x4fa0,5)+Free 返 0；
//   get_binary(0, m_items, 0x428) 失败 cMyTrace(0x4fa8,5)+Free 返 0；
//   m_field04=1；get_byte(1, useDoll) 失败 cMyTrace(0x4fb4,5)+Free 返 0；
//   m_dollCreated=(int)useDoll（movsbl 符号扩展）。put(DISPATCH_Q) 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841c08c
bool DB_LoadPrivateStore::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序照 ORIG 栈槽（低→高）：useDoll(-0x61) characNo(-0x5c)
    // r(-0x11) db(-0x10) sig(-0xc)；guard 于 exec 成功后构造（ORIG 同点）。
    char useDoll;
    int characNo;
    bool r;
    MySQL* db;
    private_store::SIG_PRIVATE_STORE* sig;
    *stream >> characNo;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("seLect store, use_doll from store where charac_no=%d",
                  characNo);
    r = db->exec(true);
    if (r != 1)
    {
        cMyTrace trace1(__PRETTY_FUNCTION__, 0x4f8b, 5);
        trace1("DB_LoadPrivateStore::dispatch, exec ERROR charac_no=%d",
               characNo);
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4f90), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<private_store::SIG_PRIVATE_STORE>();
    memset(sig, 0, 0x434);
    sig->m_characNo = characNo;
    sig->m_field04 = 0;
    if (db->get_n_rows() != 0)
    {
        r = db->fetch();
        if (r != 1)
        {
            cMyTrace trace2(__PRETTY_FUNCTION__, 0x4fa0, 5);
            trace2("DB_LoadPrivateStore::dispatch, fetch ERROR charac_no=%d",
                   characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        r = db->get_binary(0, sig->m_items, 0x428);
        if (r != 1)
        {
            cMyTrace trace3(__PRETTY_FUNCTION__, 0x4fa8, 5);
            trace3("DB_LoadPrivateStore::GetUserInven, get_binary(2) ERROR charac_no=%d",
                   characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        sig->m_field04 = 1;
        useDoll = 0;
        r = db->get_byte(1, useDoll);
        if (r != 1)
        {
            cMyTrace trace4(__PRETTY_FUNCTION__, 0x4fb4, 5);
            trace4("DB_LoadPrivateStore::Get_Use_State, get_byte ERROR charac_no=%d",
                   characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        sig->m_dollCreated = (int)useDoll;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}