void SIG_COMBO_SKILL::reset()
{
    m_characNo = 0;
    m_tree1Count = 0;
    m_tree2Count = 0;
    m_job = 0xb;
    memset(m_tree1Data, 0, 0x54);
    memset(m_tree2Data, 0, 0x54);
