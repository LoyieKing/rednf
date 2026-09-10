void SecretShopScript::clear()
{
    m_priceVarPercent = 0;   // +0x80
    m_int84 = 0;             // +0x84
    m_npcMap.clear();        // +0x00
    m_levelSections.clear(); // +0x30
    m_levelNpcMap.clear();   // +0x18
    m_dungeonNpcMap.clear(); // +0x48
