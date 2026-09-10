void CExchangeServerHitRate::IncrementCacheLoadFailCount()
{
    ++m_cacheLoadFailCount;
    // [b28d-solo-ExchangeServer_Infra] P0/N96 清绑（ORIG 0x848f36e 段同形）⇒
    // P225 while+break 四段物化。
    while (m_cacheLoadFailCount == 0)
    {
        ++m_cacheLoadFailOverflowCount;
        break;
    }
