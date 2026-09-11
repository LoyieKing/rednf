// ---------------------------------------------------------------------------
// SendResponseCharacInfo_forchina          ORIG 0x849176e
// 三档池取流（total=len+8：<=0xfff Small / <=0x3fff Default / else Big），
// 写 a、b、binary(data) 后投递 MsgQueueMgr idx=1（DISPATCH_Q）。
// 注：ORIG 返回值取 MsgQueueMgr::put 尾部 eax（`mov %eax,%ebx` 整字直传，非规范化），
// 本函数返回 put 的结果（权威头 MsgQueueMgr::put 声明 bool，转发 Stream* 版返回值）。
// 判词：else if 写成 (total > 0xfff && total <= 0x3fff) 复现 ORIG 第二分支入口的
// 冗余重测 `cmpl $0xfff; jle`（4.1 不消除短路链中已知条件，4.4 复用前次比较结果
// 需显式保留 && 前项）。check_tu 实测 identical 口径。
// ---------------------------------------------------------------------------
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
}