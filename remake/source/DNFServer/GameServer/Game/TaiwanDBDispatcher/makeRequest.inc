void DBUpdatePcroomDailyReward::makeRequest(CUser* user)
{
    // P0：Acquire 返回值右值内联进 ctor（ORIG %eax 直通，无 stream 栈槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x325), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(0xe), user->GetUID());
    MSG* msg = guard->GetInBuffer<MSG>();   // ORIG via CStreamGuard::operator->
    if (msg == 0)
    {
        // b28d-lane6 试形：P221 全表达式临时链（翻块内对象槽方向）。
        cMyTrace("static void DBUpdatePcroomDailyReward::makeRequest(CUser*)",
                 0x32b, 0)("[Taiwan, Pcroom] msg is null");
    }
    else
    {
        msg->m_id = user->get_acc_id();
        msg->m_remain = user->m_field8ec32;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}
