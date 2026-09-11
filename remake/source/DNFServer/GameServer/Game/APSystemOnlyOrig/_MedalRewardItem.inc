// _MedalRewardItem —— W 0x08895712 C1/C2 同体：memset(this,0,0xa)（尾 pad 不清）
_MedalRewardItem::_MedalRewardItem()
{
    memset(this, 0, 0xa);
}