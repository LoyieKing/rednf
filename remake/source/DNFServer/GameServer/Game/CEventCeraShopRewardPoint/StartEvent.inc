// ORIG 0x08164618：SetEventFlag(true) + cMyTrace(PRETTY, 0x13a, 0)
//   ("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
//    m_field10, m_fieldC)。
void CEventCeraShopRewardPoint::StartEvent()
{
    SetEventFlag(true);
    register int ceraPoint = m_fieldC;    // 0xc → esi（ORIG uVar1）
    register int timing = m_field10;      // 0x10 → ebx（ORIG uVar2）
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13a, 0);
    tr("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
       timing, ceraPoint);
}