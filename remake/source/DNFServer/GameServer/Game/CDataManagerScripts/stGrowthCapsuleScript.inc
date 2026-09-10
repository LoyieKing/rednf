stGrowthCapsuleScript::stGrowthCapsuleScript()
{
    // ORIG C1 0x0837114a：五个 int 成员逐个清零（非 memset）。
    m_maxGageExp = 0;
    m_gagePercent = 0;
    m_capsuleId = 0;
    m_capsuleExp = 0;
    m_capsuleLimitLv = 0;
