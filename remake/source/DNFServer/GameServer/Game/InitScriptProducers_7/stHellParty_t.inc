// ORIG stHellParty_t C1 0x891b242 W：map@0x54 构造；标量清零；三段 memset；
// 难度表五组循环清零；+0x90=0。
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0891b242
// 4.1 发射序 = map C1(+0x54) → +0x50 →
// +0x0..0xc → +0x4c → +0x10 → +0x14 → memset 0x18/0x28/0x3c → +0x90 → 清零环
//（P239：类成员 meminit 前置、其余标量 store 段按 ORIG 序落 ctor 体）；
// 原全 meminit 形在 4.4 按声明序混合发射且 m_difficultyRates()/m_partyRates()
// 另发 value-init 循环（重复清零），删除并把标量移入体内。
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
}