bool CExchangeServer::UnregisterExchangeServer(int serverId)
{
    // [b28d-solo-Exchange] 返回类型 void→bool：ORIG 成功 `mov $0x1,%eax` /
    // 失败 `mov $0x0,%eax`（0x84914dc/0x8491520），虚函数返回类型不入 mangling。
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it != m_serverMap.end())
    {
        m_serverMap.erase(it);
        // ORIG 直传立即数 1 作 CONNECT_TYPE
        if (GetExchangeServerCount((CONNECT_TYPE)1) == 0)
        {
            SetMaster();
        }
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x84914e3 `mov 0xc(%ebp),%ebx`（serverId 跨
    // ctor 保活）与 trace 槽 -0x18（4.4 具名形恒落深槽 -0x20）。
    cMyTrace("virtual bool exchange_server::CExchangeServer::"
             "UnregisterExchangeServer(int)", 0x13b, 0)
        ("UnregisterExchangeServer() channel_no(%d) failed", serverId);
    return false;
