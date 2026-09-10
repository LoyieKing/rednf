int importDeathTowerScript(const char* path, stDeathTower_t& out)
{
    bool ok = false;
    std::string token;

    if (!loadRDARScriptFile("", path))
    {
        return 0x187e;
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }

        if (token == "[apc exp weight]")
        {
            out.m_apcExpWeight = ScanFloat(&ok);    // fstps (+0x000)，float 位模式
            if (!ok) { return 0x1887; }
        }
        else if (token == "[reward exp weight]")
        {
            int i = 1;                              // ORIG 下标从 1 开始
            for (;;)
            {
                out.m_rewardExpWeight[i] = ScanFloat(&ok);   // fstps 0x4(%eax,%ebx,4)
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[reward card num]")
        {
            int i = 1;
            for (;;)
            {
                // [L16 本轮] P211 档③ 试形：RHS 显式 unsigned 与 int 字段异型
                // ⇒ 复现 ORIG RHS-first + `mov %eax,%edx` 中转（08908b06 段）。
                out.m_rewardCardNumU[i] = ScanInt(&ok);  // (0xc8+i)*4+4 → +0x324+i*4
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward exp weight]")
        {
            int i = 1;
            for (;;)
            {
                out.m_illusionRewardExpWeight[i] = ScanFloat(&ok);  // (0x190+i)*4+4 → +0x644+i*4
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[illusion reward card num]")
        {
            int i = 1;
            for (;;)
            {
                out.m_illusionRewardCardNumU[i] = ScanInt(&ok);  // P211 档③（08908ba8 段）
                ++i;
                if (!ok) { break; }
            }
        }
        else if (token == "[monster exp weight]")
        {
            out.m_monsterExpWeight = ScanFloat(&ok);  // fstps +0xc84
            if (!ok) { return 0x18b8; }
        }
        else if (token == "[reward item prob]")
        {
            // P211：字段 int 与 RHS unsigned 异型 ⇒ RHS-first + `mov %eax,%edx` 中转。
            out.m_rewardItemProbU[0] = ScanInt(&ok);   // ORIG 先存储后校验（08908a73 段）
            if (!ok) { return 0x18be; }
            // [L16 本轮] 书写序两形实测：`mem + call()` 反劣（ourN 236、ndiff 26）；
            // `call() + mem` 最优（ourN 232、ndiff 6，残 lea-vs-add 2 处 = N4/P205）。
            out.m_rewardItemProbU[1] = (unsigned int)ScanInt(&ok) + out.m_rewardItemProbU[0];
            if (!ok) { return 0x18c1; }
            out.m_rewardItemProbU[2] = (unsigned int)ScanInt(&ok) + out.m_rewardItemProbU[1];
            if (!ok) { return 0x18c4; }
        }
        else if (token == "[reward gold weight]")
        {
            out.m_rewardGoldWeight = ScanFloat(&ok);  // fstps +0xc94
            if (!ok) { return 0x18ca; }
        }
        else
        {
            return 0x18cd;                          // ORIG 未知节名即失败
        }
    }
    return 0;
