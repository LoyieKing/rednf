int ImportNewAccountLeventUpToJobScript(const char* path,
                                        stNewAccountLevelUpToJobScript& script)
{
    if (!loadRDARScriptFile("", path))
        return 0x3629;

    std::string token;
    int err = 0;                                 // ORIG -0xc（token ctor 后、clear 前清零）
    script.m_stepReward.clear();                 // ORIG +0x044（0x08371508）

    for (;;)                                     // ORIG 0x08917a53：bool callee 取反形
    {
        if (!ScanType(token, true))
            break;

        if (token == "[reward stackable item]")
        {
            // b28d-lane25：P167 差集实证 ORIG 直接传字面量（C1EPKc 临时，无 C1ERKSs 拷贝）。
            err = LoadRewardDataByNoType(script.m_stepReward, "[/reward stackable item]");
            if (err != 0)
                return err;
        }
        else if (token == "[reward equip item]")
        {
            err = LoadRewardDataByGrowType(script.m_levelUp18, "[/reward equip item]");
            if (err != 0)
                return err;
        }
        else if (token == "[reward avatar item]")
        {
            err = LoadRewardDataByGrowType(script.m_levelUp30, "[/reward avatar item]");
            if (err != 0)
                return err;
        }
        else
        {
            continue;
        }
    }
    return 0;
}
