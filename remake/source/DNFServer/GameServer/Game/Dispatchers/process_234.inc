int Dispatcher_Break_Away_Quest_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBreakAwayParam& res = static_cast<stBreakAwayParam&>(param);  // -0x10（ORIG 缓存 param）
    MSG_BREAK_AWAY& req = reinterpret_cast<MSG_BREAK_AWAY&>(msg);        // -0xc（ORIG 缓存 msg）
    if (req.m_checkFlag != 0)                                   // msg+0x0d byte（ORIG 正例 fall-through）
    {
        if (user->getBreakAwayDungeonClearCnt() == 0)
        {
            GlobalData::s_BreakAwaySys->giveBreakAwayDungeonClearReward(user);  // ORIG 0x0831d164
        }
        else
        {
            res.m_resultCode = 1;                               // param+0x04 = 1
        }
    }
    else
    {
        user->setBreakAwayDungeonClearCnt(-1);                  // CUser::setBreakAwayDungeonClearCnt(-1)
        user->setBreakAwayQuestTime(0);                         // CUser::setBreakAwayQuestTime(0)

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "PacketDispatcher_Impl_1.cpp", 0x31d9), true);      // CStreamGuard C1 0x080c8c26（P134 内联 Acquire）
        **guard << (int)0x144;                                   // operator<<(int) 0x080c8c56
        **guard << (unsigned int)user->GetUID();                 // operator<<(unsigned int) 0x080e2292
        **guard << (unsigned int)user->get_acc_id();             // 同上
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);  // MsgQueueMgr::put 0x08570fde
    }
    return 0;
