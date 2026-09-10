void CCeraShopBonusItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x26, 0);
    tr("Cera shop bonus item event End");
