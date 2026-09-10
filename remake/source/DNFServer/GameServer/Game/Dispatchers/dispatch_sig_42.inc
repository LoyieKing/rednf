int Dispatcher_GenCeraTicket::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0x91f0, __PRETTY_FUNCTION__, 0, 0);
    }
    int randSeed = get_rand_int(0xffff);                        // -0xc
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0x91f5), true);           // P134 内联 Acquire
    **guard << 0x49;                                             // ORIG operator* 链
    **guard << (int)user->GetUID();
    **guard << user->get_acc_id();
    **guard << randSeed;
    char uniqueNo[32];
    **guard << WongWork::CGenUniqueNo::GenUniqueNo(uniqueNo);   // static（无 this，无 C1 构造）
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
