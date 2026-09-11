// [P327] ORIG 的 _GLOBAL__I_ 键名为 _ZN12CAchievementC2Ev（ORIG 0x0828d853），
// 且 ORIG C1/C2 地址 0x0828b614 早于 stAchievementC1 0x0828d870 ⇒ 原码把
// CAchievement ctor 定义在本 TU 最前。GI 键名 = TU 内第一个强符号外部链接
// 实体名；把本定义前置即对齐键名（只移函数，不改构造序）。
CAchievement::CAchievement() : m_user(0)
{
    // ORIG 0x828b633：m_user 置零在 map 构造之前（初始化列表，
    // 按声明顺序先于 m_achievements）；m_field24/25 在函数体
    m_field24 = 0;
    m_field25 = 0;
}