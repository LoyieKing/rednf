// ---------------------------------------------------------------------------
// SigLoadRewardEventItem —— ORIG W 全套：
//   C1/C2 0x08186686 -> init() 0x0818669a -> set(0) 0x081866b6；
//   set(v)：m_field0=v；m_count=0；memset(m_characNos,0,0x28)；
//           memset(m_eventCodes,0,0x28)
// （memset 先 characNos 区后 eventCodes 区，次序照 asm）
// ---------------------------------------------------------------------------
SigLoadRewardEventItem::SigLoadRewardEventItem()
{
    init();
}