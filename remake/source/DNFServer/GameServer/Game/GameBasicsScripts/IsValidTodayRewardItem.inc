bool APSystem::CActionPointEtcParameter::IsValidTodayRewardItem(
    _SIG_LOAD_ACTION_POINT::_TodayRewardItem item)
{
    // ORIG 0x08894976：value 无槽（test %eax/js + 寄存器 cmp/jg 直判，P0 右值内联）。
    return item.m_itemIdx >= 0 && item.m_itemIdx <= 3;
