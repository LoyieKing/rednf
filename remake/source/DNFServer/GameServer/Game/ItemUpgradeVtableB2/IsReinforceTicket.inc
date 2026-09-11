// ============================================================================
// CStackableItem 强化/增幅券访问器（0xf 字节级访问器；声明见 CStackableItem.h）
//   IsReinforceTicket      W 0x084b4222：movzbl +0x30f（m_char30f）
//   GetReinforceRate       W 0x084b4232：movzbl +0x312（m_char312）
//   IsAmplifyReinforceTicket W 0x084b4242：movzbl +0x313（m_char313）
//   GetAmplifyReinforceRate  W 0x084b4252：movzbl +0x316（m_char316）
//   GetReinforceVaule      T 0x0854b4f8：movzwl +0x310（m_short310）
//   GetAmplifyReinforceValue T 0x0854b508：movzwl +0x314（m_short314）
// ============================================================================
bool CStackableItem::IsReinforceTicket()
{
    return m_char30f;
}