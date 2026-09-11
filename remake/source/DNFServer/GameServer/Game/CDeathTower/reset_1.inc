void CDeathTower::CStage::reset()
{
    // ORIG 0x84614ec：MapInfo::Clear(m_mapInfo)；m_assignItemIdx/m_dropItemCnt
    // = get_rand_int(60000)；m_state = 0。
    m_mapInfo.Clear();
    m_mapInfo.m_assignItemIdx = get_rand_int(60000);
    m_mapInfo.m_dropItemCnt = get_rand_int(60000);
    m_state = (ENUM_STAGE_STATE)0;
}