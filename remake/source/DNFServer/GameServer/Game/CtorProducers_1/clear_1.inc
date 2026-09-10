void WorldMapScript::clear()
{
    m_index = 0;
    m_name = "";              // ORIG 0x08369e74W：operator=(const char* "")（_ZNSsaSEPKc）
    m_mapImage = "";
    // ORIG clear 不清 m_uiPath(+0xc)；仅清 +0x4/+0x8 两个 string。
    m_field10 = 0;
    m_dungeons.clear();
    m_isHellDungeon = 0;
    m_hellQuests.clear();
    m_freepassItems.clear();
    m_itemConditions.clear();
    m_minimapPoints.clear();
