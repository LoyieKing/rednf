// SendResult —— 报告全文转写。ORIG T 0x08124b8e。
bool DB_LoadRewardMedal::SendResult(int a, int b, _SIG_LOAD_REWARD_MEDAL& sig)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localchina/localchina_ActionPointSystem_Dispatcher.cpp", 0x1c0),
                       true);
    guard.operator*()->operator<<(a);
    guard.operator*()->operator<<(b);
    guard.operator->()->put_binary(&sig, 0x13);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}