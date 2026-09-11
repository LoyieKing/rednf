// ---------------------------------------------------------------------------
// ExchangeServerInfo::ExchangeServerInfo   ORIG 0x8491004（逐域清零）
// ---------------------------------------------------------------------------
ExchangeServerInfo::ExchangeServerInfo()
    : m_field00(0), m_word04(0), m_type08(EXCHANGE_CONNECT_UNKNOWN),
      m_pSession(NULL)
{
}