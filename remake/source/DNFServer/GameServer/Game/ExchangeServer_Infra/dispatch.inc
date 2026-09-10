void CExchangeServerThread::dispatch(void* arg)
{
    (void)arg;
    // [b28d-solo-ExchangeServer_Infra] P221/P227：ORIG 四个 cMyTrace 槽
    // -0x50/-0x40/-0x30/-0x20（深→浅，按出现序）；具名对象形 4.4 恒反向
    // （-0x20/-0x40/-0x50/…）⇒ 改「构造即调用」全表达式临时链复现 ORIG 槽向。
    cMyTrace(__PRETTY_FUNCTION__, 0x451, 0)(
        "Exchange Server Thread start! %d",
        static_cast<int>(sizeof(CACHE_CHARACTER_TYPE)));

    if (!m_exchangeServer.PreparePassiveSocket())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x459, 0)(
            "\n\n\n\n\n\nExchange Server starting error!\n\n\n\n\n\nShudown~~");
        exit(-1);
    }

    if (!m_exchangeServer.SendExchangeServerInfo())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x469, 0)("SendExchangeServerInfo failed!");
        return;                       // ORIG jmp 尾部
    }

    while (!GlobalData::s_server_stop)         // 标志 @0x0940bd64
    {
        // P15 赋值即条件（同 WaitForEvent OnAccept 判据）。
        Stream* msg = 0;
        if ((msg = m_exchangeServer.GetMessage()) != 0)
        {
            // flag 取流缓冲首 dword（ORIG mov (%eax) 后作为 FreeMessage 第三参）。
            int flag = *reinterpret_cast<int*>(msg->get_buffer());
            // [b28d-solo-ExchangeServer_Infra] P277：ORIG 0x8493d1a/0x8493d2e 段
            // 先算 this（`mov 0x8(%ebp),%eax; lea 0xc(%eax),%edx`）再装实参 ⇒
            // receiver 走 cast 前置形；隐式成员访问 4.4 恒先装实参（add $0xc 形）。
            m_exchangeServer.DispatchMessage(msg);
            m_exchangeServer.FreeMessage(msg, flag);
        }
        m_exchangeServer.GetNetworkEvent();
        OS_API::Sleep(0xa);                        // ORIG 10ms 节拍
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x482, 0)("Exchange Server Thread End!");
