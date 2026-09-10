int TimerConnectMonitorServer::dispatch_sig(int /*p1*/, int p2, unsigned int /*p3*/)
{
    // ORIG 0x086380dc：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)（无 stream 槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                          0xeaa), true);

    // ORIG 0x086380f8/0x08638113：`*guard`（deEv 0x080c8c46）+ lsEi 两级。
    (*guard)->operator<<(0x121);
    (*guard)->operator<<(-1);

    // ORIG 0x0863812e/0x08638139：`guard->`（ptcEv 0x080c8c4e）+ GetInBuffer。
    Packet_Tcp_Server_Connect* pkt = guard->GetInBuffer<Packet_Tcp_Server_Connect>();
    // ORIG 0x08638144：值源为 p2（0x10(%ebp)），具名 char 字段恢复
    // `mov %dl,0xa(%eax)` disp 直存（0x0863814c）。
    pkt->m_serverFlag = static_cast<char>(p2);

    // ORIG +0xaa: MsgQueueMgr::put(s_msgq_mgr, 1, guard)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // 析构由作用域自然产生（ORIG +0xba ~CStreamGuard）
    return 1;
