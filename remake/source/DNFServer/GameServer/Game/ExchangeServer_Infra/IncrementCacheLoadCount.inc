// IncrementCacheLoadCount ORIG 0x823468a：+4 自增，归零则 (+0xc) +1。
void CExchangeServerHitRate::IncrementCacheLoadCount()
{
    ++m_cacheLoadCount;
    // [b28d-solo-ExchangeServer_Infra] P0/N96 清绑（同 IncrementDBLoadCount，
    // ORIG 0x823469a 段同形 `test; sete; test; je` 无槽）⇒ P225 while+break。
    while (m_cacheLoadCount == 0)
    {
        ++m_cacheLoadOverflowCount;
        break;
    }
}