// ORIG 0x081646b8：cMyTrace(PRETTY, 0x147, 0) + tr("[Taiwan, CeraShopRewardPoint]
// End event.") + 清 m_field10 / m_fieldC + SetEventFlag(false)。
void CEventCeraShopRewardPoint::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x147, 0);
    tr("[Taiwan, CeraShopRewardPoint] End event.");
    m_field10 = 0;
    m_fieldC = 0;
    SetEventFlag(false);
}