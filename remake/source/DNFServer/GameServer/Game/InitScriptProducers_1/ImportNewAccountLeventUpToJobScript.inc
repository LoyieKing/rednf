// b28d-solo-InitScriptProducers_1: DIFF→AE（0x2ae=0x2ae）。修：共享的链外
// `if (err != 0) return err;` 移入三个分支体各一份 —— ORIG 每分支独立
// `cmpl $0,-0xc; je <岛>` + fall-through return（0x08917af8/0x08917ba7/0x08917c53，
// 岛 0x08917a36/39/3f 各 `nop; jmp 循环头`）；共享检查在 4.4 下 tail-merge 成单回边。
// err 具名 int（ORIG -0xc）与 token→err→clear 声明序已复原。
// ORIG T 0x089179eb。首成员 clear 后按三段分发至共享 loader；loader 非 0 即透传。
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
