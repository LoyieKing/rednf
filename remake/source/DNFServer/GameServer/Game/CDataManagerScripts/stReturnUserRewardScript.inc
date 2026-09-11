// ---- stReturnUserRewardScript / stBingoScript / stNewAccountLevelUpToJobScript----
// ORIG C1 0x8371544：两个 int 置 0 后构造 + clear 内嵌 stLevelUpRewardItemScript。
stReturnUserRewardScript::stReturnUserRewardScript()
    : m_field0(0), m_field4(0)
{
    m_levelUp.clear();   // ORIG C1 0x8371544 调 stLevelUpRewardItemScript::clear()
}