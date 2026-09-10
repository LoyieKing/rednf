void CEventCeraShopRewardPoint::StartEvent()
{
    SetEventFlag(true);
    register int ceraPoint = m_fieldC;    // 0xc → esi（ORIG uVar1）
    register int timing = m_field10;      // 0x10 → ebx（ORIG uVar2）
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13a, 0);
    tr("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
       timing, ceraPoint);
