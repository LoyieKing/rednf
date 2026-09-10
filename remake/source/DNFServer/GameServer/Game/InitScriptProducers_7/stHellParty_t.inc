stHellParty_t::stHellParty_t()
    : m_groupMap()
{
    m_groupIndex = 0;                        // +0x50
    m_maxBaseValue = 0;                      // +0x00
    m_minBaseValue = 0;                      // +0x04
    m_baseValue = 0;                         // +0x08
    m_baseRisingValue = 0;                   // +0x0c
    m_ancientDungeonAddRate = 0.f;           // +0x4c
    m_subBaseRates = 0.f;                    // +0x10
    m_passSubBaseRates = 0.f;                // +0x14
    memset(m_levelDiffRates, 0, sizeof(m_levelDiffRates));     // +0x18
    memset(m_difficultyRates, 0, sizeof(m_difficultyRates));   // +0x28
    memset(m_partyRates, 0, sizeof(m_partyRates));             // +0x3c
    m_guideLineValue = 0;                    // +0x90
    for (int i = 0; i <= 5; ++i)
    {
        m_field6c[i] = 0;
        m_field72[i] = 0;
        m_field7e[i] = 0;
        m_field84[i] = 0;
        m_field8a[i] = 0;
    }
