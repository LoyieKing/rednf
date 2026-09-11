// ---------------------------------------------------------------------------
// GetExchangeServerCount                   ORIG 0x84910ca
// （遍历 map 比较 value.m_type08 == type）
// ---------------------------------------------------------------------------
int CExchangeServer::GetExchangeServerCount(CONNECT_TYPE type)
{
    int count = 0;
    // P212 头部声明 + P13 缓存 end：ORIG 头段 begin→-0x10、end→-0x14 各求值一次，
    // 4.4 头部声明先声明浅 ⇒ it=-0x10/e=-0x14 精确命中（旧 for-init + 条件内
    // 重调 end() 致循环体多 5 条重评线）。
    std::map<int, ExchangeServerInfo>::iterator it = m_serverMap.begin();
    std::map<int, ExchangeServerInfo>::iterator end = m_serverMap.end();
    for (; it != end; ++it)
    {
        if (it->second.m_type08 == type)
        {
            ++count;
        }
    }
    return count;
}