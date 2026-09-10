float CPowerManager::GetPowerWarResponPenalty(CUser* user)
{
    // ORIG 0x08480ec8：penalty 初始 1.0；混沌时间 < 阈值时二次读取
    // GetCurCharacChaosStateTime / G_CDataManager 计算 2.0 - chaos/threshold。
    float penalty = 1.0f;
    if ((unsigned int)user->GetCurCharacChaosStateTime() <
        (unsigned int)G_CDataManager()->m_powerParameter.m_penaltyThreshold)
    {
        // [b28c-L9] ORIG 0x08480f03：`mov $0,%edx; mov %eax,-0x20; mov %edx,-0x1c;
        // fildll` = unsigned int→float 的 64 位零扩展路径（P196 符号性）。
        penalty = 2.0f -
                  (float)(unsigned long long)(unsigned int)user->GetCurCharacChaosStateTime() /
                      (float)G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    }
    return penalty;
