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
