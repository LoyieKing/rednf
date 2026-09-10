GenuineDamageUpgradeInfo::GenuineDamageUpgradeInfo()          // ORIG W 0x0891baf0
{
    m_a = 0;
    m_b = 0;
    m_levelRate = 0.0f;     // +0x08（ctor 清零；import 写 level/10000f）
    m_goldRate = 0.0f;      // +0x0c；+0x10 保持未初始化（ORIG 同）
