// ORIG 0x0815c986 sz 0x10d（_ZN25DBSelectPcroomDailyReward11makeRequestEP5CUser）：
// Acquire("localtaiwan/Dispatcher/Taiwan_DbThread.cpp",0x2d9) → CStreamGuard
//   → Taiwan::internal_stream(guard, T(13), user->GetUID())
//     （实测 2026-08-30：ORIG 0x0815c9c8 实址 call _ZNK5CUser6GetUIDEv，非
//      get_acc_id；NEW 误用 get_acc_id 已修正为 GetUID。GetUID=slotIDX<<16|field704a6）
//   → GetInBuffer<MSG>()，null 走 cMyTrace("[Taiwan, Pcroom] msg is null")，
//   否则 *(uint*)msg(+0x00)=user->get_acc_id()；MsgQueueMgr::put(DB_Q(2), guard)。
void DBSelectPcroomDailyReward::makeRequest(CUser* user)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x2d9),
                       true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)13,
                            (int)user->GetUID());
    MSG* msg = guard->GetInBuffer<MSG>();
    if (msg == 0)
    {
        // ORIG 0x0815c…：trace 临时对象落近槽 -0x1c、guard 保持 -0x24
        // （具名 trace 使 4.4 两槽互换，QueueSizeLog/CountOfHumanCertify
        // 同法 P37）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2df, 0)(
            "[Taiwan, Pcroom] msg is null");
    }
    else
    {
        // ORIG：*(uint*)msg = accId（MSG +0x00 account id 字段）
        msg->m_id = user->get_acc_id();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}