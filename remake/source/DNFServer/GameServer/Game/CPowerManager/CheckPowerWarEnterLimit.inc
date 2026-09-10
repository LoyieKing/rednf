int CPowerManager::CheckPowerWarEnterLimit(CUser* user)
{
    int minLevel = G_CDataManager()->m_powerParameter.m_minEnterLevel;
    int minAge = G_CDataManager()->m_powerParameter.m_minEnterAge;
    if (minLevel == 0)
        minLevel = 0x1e;
    if (minAge == 0)
        minAge = 0xf;
    // ORIG 直调 CUser::CheckLimitMiniumAge（T 0x0864fc3e），禁内联展开（§4.1）。
    int outUnique;    // ORIG 不预清 -0x14（P198①：出参槽仅 lea，无 movl $0）
    // ORIG `xor $1;test;je`：`if (!bool)` 翻转直测；`==1` 反成 `test;je`（P196 极性）。
    if (!user->CheckLimitMiniumAge(minAge, outUnique))
    {
        return 0x5f;
    }
    if (user->get_charac_level() < minLevel)
        return 0xe;
    return 0;
