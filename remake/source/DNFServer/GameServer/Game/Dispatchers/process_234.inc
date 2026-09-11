// ORIG 0x081d390a：msg.m_checkFlag==0（+0xd）→ setBreakAwayDungeonClearCnt(-1)、
// setBreakAwayQuestTime(0)、StreamPool::Acquire("PacketDispatcher_Impl_1.cpp",
// 0x31d9)+CStreamGuard(stream,true) 写 0x144/UID/acc_id 入 DB_Q；
// 否则 getBreakAwayDungeonClearCnt()==0 →
// GlobalData::s_BreakAwaySys->giveBreakAwayDungeonClearReward(user)，
// 非 0 → param.m_resultCode=1。
// 【blocked：giveBreakAwayDungeonClearReward 及其 selectReward/
// Inter_LoadEtc::giveBreakAwayReward 生产链尚未还原（报告齐备，独立批次），
// 已 hub 通知 Main；不写猜测实现。】
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d390a orig/ours=98/98
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
        // [P4-p167 符号性] ORIG 第 11 个 call = operator<<(int)：GetUID 返回
        //   unsigned int（CUser.h:3088，共享声明不动），故调用点显式转 (int)。
        **guard << (int)user->GetUID();                          // operator<<(int) 0x080e2292
        **guard << (unsigned int)user->get_acc_id();             // 同上
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);  // MsgQueueMgr::put 0x08570fde
    }
    return 0;
}