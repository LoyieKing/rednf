bool CExchangeServer::SendResponseCharacInfo_forchina(int a, int b,
                                                      void* data, int len)
{
    CStreamGuard sGuard(true);
    int total = len + 8;
    if (total <= 0xfff)
    {
        sGuard.Set(GlobalData::s_small_stream_pool->Acquire("ExchangeServer.cpp",
                                                0x17a));
    }
    else if (total > 0xfff && total <= 0x3fff)
    {
        sGuard.Set(GlobalData::s_stream_pool->Acquire("ExchangeServer.cpp", 0x17e));
    }
    else
    {
        sGuard.Set(GlobalData::s_big_stream_pool->Acquire("ExchangeServer.cpp",
                                              0x182));
    }

    // ORIG 显式走 CStreamGuard::operator*()（deEv，返回 this 指针）再在其上调
    // operator<<(int)（lsEi）；put_binary 走 operator->()（ptEv）——P103 链不可省略。
    sGuard.operator*()->operator<<(a);
    sGuard.operator*()->operator<<(b);
    sGuard.operator->()->put_binary(data, len);

    // ORIG mov %eax,%ebx：直接返回 MsgQueueMgr::put 的结果（非硬编码 true）。
    return GlobalData::s_msgq_mgr->put(QIdx(1), sGuard);
