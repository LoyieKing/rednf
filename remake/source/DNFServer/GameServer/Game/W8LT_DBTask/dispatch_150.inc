bool DB_InsertCreatureItem::dispatch(int /*userIdx*/, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    // 4.4 槽分配 = 先声明占更低槽、block 对象续顶向下：声明序 sig/db/out →
    // sig(-0x24) db(-0x20) out(-0x1c) guard(-0x2c)，对齐 ORIG 0x841f8xx。
    SIG_CREATURE_ITEM* sig;    // -0x24
    MySQL* db;                 // -0x20
    SIG_CREATURE_ITEM* out;    // -0x1c
    sig = stream->GetOutBuffer<SIG_CREATURE_ITEM>();
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    if (!insertCreatureItem(db, sig))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x55c8), true);
    guard.operator*()->operator<<((int)0x88);
    guard.operator*()->operator<<((int)slot);
    out = guard.operator->()->GetInBuffer<SIG_CREATURE_ITEM>();
    *out = *sig;   // 结构整体赋值 → rep movsl 0x1d（P83；memcpy 会 call）
    out->m_addInfo2 = WongWork::DBCommon::GetIdentity(db);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
