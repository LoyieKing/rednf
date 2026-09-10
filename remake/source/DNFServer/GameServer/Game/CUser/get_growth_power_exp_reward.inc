STExpReward CUser::get_growth_power_exp_reward()
{
    // ORIG 把 CUser+0x8d24d 的 dword 直接写入 sret（无 STExpReward ctor，9 insns）。
    // 真值成员 return 直写 = 4.4 唯一复现形（probe g5/g6 实证；cast/引用/临时均 memcpy）。
    return m_reward;
