int Dispatcher_P2PHolePunchingSuccessRateStatistics::process(CUser* user, MSG_BASE& msg,
                                                             ParamBase& param)
{
    (void)param;
    // ORIG 0x0825ffb6：Acquire("PacketDispatcher_Impl_2.cpp",0xb4c) → internal_stream(0xf,uid)
    // → GetInBuffer<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE> ← memcpy(&msg,0x16) → QUEUE 1
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_2.cpp", 0xb4c), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0xf, (int)user->GetUID());
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE* in =
        guard->GetInBuffer<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE>();
    memcpy(in, &msg, 0x16);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}