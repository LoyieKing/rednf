// ---------------------------------------------------------------------------
// RegisterExchangeServer                   ORIG 0x849135c
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：P293 具名 it（find 先/end 后）+ P221 临时链保活。
bool CExchangeServer::RegisterExchangeServer(int serverId,
                                             ExchangeServerInfo& info)
{
    // [b28d-solo-Exchange] P293：ORIG 0x849137a 先 find（it=-0x50）后 end
    // （临时 -0x4c）⇒ 具名 it 形（单表达式 find()==end() 在 4.4 反产 end 先）。
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it == m_serverMap.end())
    {
        m_serverMap.insert(std::make_pair(serverId, info));
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x8491404 `mov 0xc(%ebp),%ebx` + trace 槽 -0x18。
    cMyTrace(__PRETTY_FUNCTION__, 0x128, 0)
        ("RegisterExchangeServer() channel_no(%d) failed", serverId);
    return false;
}