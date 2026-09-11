bool Inter_LoadEtc::RequestDBEventArad(CUser* user)
{
    arad::SigLoadRewardEventItem sigEventItem;
    sigEventItem.set(user->get_acc_id());           // ORIG W 0x081866b6

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x1193),
                       true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard,
        ARAD::ENUM_INTERNALPACKET_JPN(0x26), (int)user->GetUID());
    guard.operator->()->put_binary(&sigEventItem, sizeof(sigEventItem));  // ptEv→0x56
    // [b24ab] ORIG `mov %eax,%ebx` 直接返回 put 结果（32 位），非 bool `ok!=false`
    // （后者多 mov%al/movzbl 规范化 + 帧扩容）
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}