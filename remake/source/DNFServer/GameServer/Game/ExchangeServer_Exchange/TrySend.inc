// ---------------------------------------------------------------------------
// TrySend                                  ORIG 0x84911fc
// （map[serverId].m_pSession 存在则经 SessionManager 转发）
// ---------------------------------------------------------------------------
bool CExchangeServer::TrySend(int serverId, PacketGuard& guard)
{
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    // P168a③b 实测定稿（探针 /tmp/l11_x.cpp f7）：合并 && 使 4.4 出 mov$1/mov$0
    // 岛；**嵌套 if** 形下第二条件出 ORIG 的 `test; setne %al; test; je <共享FAIL尾>`
    // 物化形，bool callee(ne) 第一条件直 `test %al; je` 与 ORIG 全等。
    if (it != m_serverMap.end())
    {
        if (it->second.m_pSession != 0)
        {
            return m_sessionMgr.TrySend(it->second.m_pSession, guard);
        }
    }
    return false;
}