bool CExchangeServer::ForwardRequestCharac_forchina(int a, int b,
                                                    void* data, int len)
{
    // [b24ab] ORIG `cmpl $0x20,0xc(%ebp)` 检查首参 a（this@8,a@c,b@10,data@14,
    // len@18），非 len（len@18 供 total=len+8）。原 `if(len==0x20)` 语义 bug。
    if (a == 0x20)
    {
        GetInstanceExchangeServerHitRate()->IncrementCacheLoadFailCount();
    }

    CStreamGuard sGuard(true);
    int total = len + 8;
    if (total <= 0xfff)
    {
        sGuard.Set(GlobalData::s_small_stream_pool->Acquire("ExchangeServer.cpp",
                                                0x1ec));
    }
    else if (total > 0xfff && total <= 0x3fff)
    {
        sGuard.Set(GlobalData::s_stream_pool->Acquire("ExchangeServer.cpp", 0x1f0));
    }
    else
    {
        sGuard.Set(GlobalData::s_big_stream_pool->Acquire("ExchangeServer.cpp",
                                              0x1f4));
    }

    sGuard.operator*()->operator<<(a);       // deEv → op<<(int)
    sGuard.operator*()->operator<<(b);       // deEv → op<<(int)
    sGuard.operator->()->put_binary(data, len);  // ptEv → put_binary

    // [b24ab] ORIG `call put; mov %eax,%ebx; ... mov %ebx,%eax; ret` 直接返回
    // put 结果（非硬编码 true），与 _forchina 兄弟同构。
    return GlobalData::s_msgq_mgr->put(QIdx(2), sGuard);
