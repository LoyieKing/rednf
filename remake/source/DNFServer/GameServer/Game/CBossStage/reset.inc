// ORIG 0x0814bea2：m_startTick=0、m_endTick=0、MapInfo::Clear、
//   memset(m_useItemCount, 0, 0x10)（CBossPlay::m_useItemCount[4]，ORIG this+0x9c8）。
void CBossStage::reset()
{
    m_startTick = 0;
    m_endTick = 0;
    m_stageData.Clear();
    memset(m_useItemCount, 0, sizeof(m_useItemCount));
}