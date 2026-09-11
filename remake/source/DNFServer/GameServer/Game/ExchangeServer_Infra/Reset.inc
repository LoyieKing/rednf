// Reset ORIG 0x82346bc：六槽清零。
void CExchangeServerHitRate::Reset()
{
    m_dbLoadCount = 0;
    m_cacheLoadCount = 0;
    m_dbLoadOverflowCount = 0;
    m_cacheLoadOverflowCount = 0;
    m_cacheLoadFailCount = 0;
    m_cacheLoadFailOverflowCount = 0;
}