channel_info_t::channel_info_t()
    : m_name(),
      m_dungeonName(),
      m_dungeonKey()
{
    m_channelType = 0;
    m_pBonusRates = 0;
    m_bonusRate = 0;
    memset(m_gridPercent, 0, sizeof(m_gridPercent));
}
