int CMission::_Reward(CUser& user) const
{
    // 基类默认实现：ORIG 弱符号 0x85e6950，仅返回 1。
    // 派生类可覆写以执行具体奖励逻辑（见 CMission_combo_clear::_Reward）。
    (void)user;
    return 1;
