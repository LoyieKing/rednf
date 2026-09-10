void upgradeAbilityBloodSystem(int idx, ActiveStaticInfo* info)
{
    // ORIG 0x088b917b：全局表直接索引（无 tbl 指针变量）。
    info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                      BLOOD_SYSTEM_BONUS[0][idx + 0x14]);
    info->m_attr17Base[13] += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0xf]);
    info->m_statBase6 += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x32]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x23]);
    }
    info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                         BLOOD_SYSTEM_BONUS[0][idx + 0x19]);
    info->m_statBase3 += static_cast<int>(BLOOD_SYSTEM_BONUS[0][idx + 0x1e]);
    info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x28]);
    info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x28]);
    info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x2d]);
    info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                          BLOOD_SYSTEM_BONUS[0][idx + 0x2d]);
