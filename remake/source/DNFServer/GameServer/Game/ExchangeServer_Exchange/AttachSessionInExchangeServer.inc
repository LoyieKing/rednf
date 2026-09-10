bool CExchangeServer::AttachSessionInExchangeServer(int serverId,
                                                    CSession* session)
{
    // [b28d-48 复核] 维持 N93附+P75（NDIFF 53，53/55 条）：iterator 对槽深
    // 方向 + session 形参跨 cMyTrace ctor 保活差。
    // [b28c-6] fresh 复验维持 N93附+P75（ndiff 22，55/53 条）：
    // [b28-L11 判 N·N93附+P75] 残差两类：①函数级 iterator 对(-0x20/-0x1c)与
    // if 尾块内 cMyTrace(-0x18) 的槽深方向跨版本相反（N93 附：兄弟/块内对象
    // 分配方向，P212 头部声明法对带参 ctor 类不可用）；②ORIG push %ebx 将
    // session 形参跨 cMyTrace ctor 调用保活（`mov %ebx,0x8(%esp)`），4.4 恒在
    // 组参点重读形参槽（P75/N82附 多实参首值寄存器对调）——ndiff 22 封顶。
    session->SetChannelNo(serverId);
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it != m_serverMap.end())
    {
        it->second.m_pSession = session;
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x8491314 `mov 0xc(%ebp),%ebx` + trace 槽 -0x18。
    cMyTrace("bool exchange_server::CExchangeServer::"
             "AttachSessionInExchangeServer(int, "
             "exchange_server::CSession*)", 0x11b, 0)
        ("AttachSessionInExchangeServer() channel_no(%d) failed", serverId);
    return false;
