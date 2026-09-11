// ---------------------------------------------------------------------------
// ORIG 0x08117270：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x22,0)
// ("CReduceUpgradeItemPay Event End!")。
void CReduceUpgradeItemPay::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CReduceUpgradeItemPay Event End!");
}