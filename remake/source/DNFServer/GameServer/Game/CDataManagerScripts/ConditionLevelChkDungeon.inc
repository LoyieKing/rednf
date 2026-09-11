// ---- ConditionLevelChkDungeon / FairPvPStatScript / RegenerationROI ----
// ORIG C1 0x837c9ac / D1 0x837c9c0（本 TU 唯一 owner）：仅构造/析构 +0x00 的
// vector<int>；objdump 直转，标量成员无动作。
ConditionLevelChkDungeon::ConditionLevelChkDungeon()
    : m_unableDungeons()
{
}