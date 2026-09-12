// ---------------------------------------------------------------------------
// ORIG 0x08117270：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x22,0)
// [R11fixE5] ORIG 0x081172ac 串=0x8b4ddcc（raw 逐字节
// 'CPowerWarRewardRandomBuffEvent Event End!' —— ORIG 自身复制粘贴缺陷，照抄）。
void CReduceUpgradeItemPay::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
    tr("CPowerWarRewardRandomBuffEvent Event End!");
}