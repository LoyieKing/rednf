// _TodayRewardItem（顶层）—— W 0x08895736 C1/C2 同体：memset(this,0,0xe)；D1/D2 空体
_TodayRewardItem::_TodayRewardItem()
{
    memset(this, 0, sizeof(*this));   // ORIG 常量 0xe
}