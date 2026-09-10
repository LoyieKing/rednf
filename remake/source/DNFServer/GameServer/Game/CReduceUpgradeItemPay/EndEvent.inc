void CReduceUpgradeItemPay::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CReduceUpgradeItemPay Event End!");
