bool importPvpMissionScript(MissionScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_PvpMissionScriptBaseDirectory, path))
        return false;

    script->clear();
    script->m_field0c =
        g_PvpMissionScriptFileList.FindIndexByFullScan(path);

    std::string token;
    std::string sub;
    std::string text;

    // [R26D] 尾置条件形（与 importPvpMissionParameterScript 的 ORIG 形相反）：
    // ORIG 0x8a5ac9c 入口 `jmp +0xb26` 直落函数尾的条件块（+0xb26 `test %al,%al;
    // jne +0x79` 成功回体、失败 +0xb47 `mov $1,%ebx` 返回 true）；写成
    // `for(;;)+if(!ScanType) return true` 会被 4.4 生成头置条件（我方旧形 +0x7d）。
    while (ScanType(token, true))
    {
        if (token == "[rank range]")
        {
            script->m_grade.first = ScanInt((bool*)0);    // +0x10
            script->m_grade.second = ScanInt((bool*)0);   // +0x14
        }
        else if (token == "[kind]")
        {
            script->m_field08 = ScanInt((bool*)0);    // +0x08
        }
        else if (token == "[rate range]")
        {
            script->m_rate.first = ScanInt((bool*)0);     // +0x18
            script->m_rate.second = ScanInt((bool*)0);    // +0x1c
        }
        else if (token == "[grade]")
        {
            ScanStr(&token);   // ORIG 复用同一 token 缓冲
            if (token == "[normal]")
                script->m_field20 = 0;
            else if (token == "[repeatable]")
                script->m_field20 = 1;
            else if (token == "[daily]")
                script->m_field20 = 2;
            else if (token == "[rank up]")
                script->m_field20 = 3;
        }
        else if (token == "[target character]")
        {
            if (!ScanStr(&sub))
                return true;                     // ORIG 扫描失败→正常收尾
            if (!checkValiedJobName(sub.c_str()))
            {
                printf("parsing error : %s\n", sub.c_str());
                return false;
            }
            script->m_job = getCharacterJob(sub.c_str());   // +0x24
        }
        else if (token == "[pre required mission]")
        {
            for (;;)
            {
                bool ok = false;
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_field28.push_back(idx);       // +0x28
            }
        }
        // [R26D] P167 一击命中：ORIG 调 `_ZSteqIcSt11char_traitsIcESaIcEEbRKSb
        // IT_T0_T1_EPKS3_`（string==字面量）而旧文写 `token != "[condition]"`
        // 生成 `_ZStne...`（MISSING eq -1 / EXTRA ne +1）。取反写回 eq 形。
        else if (!(token == "[condition]"))
        {
            if (token == "[prev mission]")
                script->m_prevMissionIndex = ScanInt((bool*)0);   // +0x04
            else if (token == "[next mission]")
                script->m_field00 = ScanInt((bool*)0);            // +0x00
            else if (token == "[name_text]")
            {
                ScanStr(&text);
                script->m_str38 = text;                           // +0x38
            }
            else if (token == "[cond_text]")
            {
                ScanStr(&text);
                script->m_str3c = text;                           // +0x3c
            }
            else if (token == "[reward item]")
            {
                for (;;)
                {
                    bool ok = false;
                    int itemIdx = ScanInt(&ok);
                    if (!ok)
                        break;
                    int count = ScanInt(&ok);
                    if (!ok)
                        return false;
                    script->m_rewardItems.push_back(
                        std::make_pair(itemIdx, count));          // +0x44
                }
            }
            else if ("[reward skill]" == token)
                script->m_rewardSkill = 1;                        // +0x5d
            else if ("[reward sp]" == token)
                script->m_rewardSp = 1;                           // +0x5e
            else if (token == "[difficulty_ratio]")
                script->m_expRate = ScanFloat((bool*)0);          // +0x40
            else if (token == "[stealing skill]")
                script->m_stealingSkill = 1;                      // +0x5c
        }
        else
        {
            // ---- [condition] 子段 ----
            if (!ScanStr(&sub))
                return true;                 // ORIG 扫描失败→正常收尾

            if (sub == "[winning count]")         { script->m_field34 = 0x0; _ReadThreeInt(script); }
            else if (sub == "[winning streak]")   { script->m_field34 = 0x1; _ReadThreeInt(script); }
            else if (sub == "[join]")             { script->m_field34 = 0x2; _ReadThreeInt(script); }
            else if (sub == "[revenge]")          { script->m_field34 = 0x7; _ReadThreeInt(script); }
            else if (sub == "[kill]")             { script->m_field34 = 0x9; _ReadThreeInt(script); }
            else if (sub == "[kill count]")       { script->m_field34 = 0x3; _ReadThreeInt(script); }
            else if (sub == "[hereafter join]")   { script->m_field34 = 0x4; _ReadThreeInt(script); }
            else if (sub == "[hereafter winning count]") { script->m_field34 = 0x5; _ReadThreeInt(script); }
            else if (sub == "[remain HP]")        { script->m_field34 = 0x6; _ReadThreeInt(script); }
            else if (sub == "[hereafter kill count]") { script->m_field34 = 0x8; _ReadThreeInt(script); }
            else if ("[within]" == sub)           { script->m_field34 = 0xb; _ReadFourInt(script); }
            else if (sub == "[remainS HP]")       { script->m_field34 = 0xc; _ReadThreeInt(script); }
            else if ("[move_channel]" == sub)     { script->m_field34 = 0xd; _ReadThreeInt(script); }
            else if ("[move_channel_total]" == sub) { script->m_field34 = 0xe; _ReadThreeInt(script); }
            else if ("[combo clear]" == sub)
            {
                script->m_field34 = 0xf;
                std::string comboTok;
                for (;;)
                {
                    if (!ScanType(comboTok, true))
                        break;               // 内层 EOF → 回外层关键字循环

                    combo_script_key key;    // ctor：job=0xb/growType=0
                    combo_script_data data;  // ctor

                    if ("[job]" == comboTok)
                    {
                        if (!ScanStr(&sub))
                        {
                            printf("parsing error %d, %s\n", 0x129,
                                   sub.c_str());
                            return false;
                        }
                        if (!checkValiedJobName(sub.c_str()))
                        {
                            printf("parsing error : %s\n", sub.c_str());
                            return false;
                        }
                        key.m_job = (char)getCharacterJob(sub.c_str());
                        key.m_growType = (short)ScanInt((bool*)0);
                        int n = ScanInt((bool*)0);
                        std::pair<int, int> fill;
                        data.m_rewardList.resize(n, fill);
                        for (int i = 0; i < n; ++i)
                        {
                            data.m_rewardList[i].first = ScanInt((bool*)0);
                            data.m_rewardList[i].second = ScanInt((bool*)0);
                        }

                        if (!ScanType(comboTok, true))
                            continue;        // ORIG esi=1 → 循环头再读一次

                        if ("[reward item]" == comboTok)
                        {
                            // ORIG 读入两个 int 后未使用（忠实转写）。
                            (void)ScanInt((bool*)0);
                            (void)ScanInt((bool*)0);
                        }
                        (void)ScanType(comboTok, true);   // 再读一枚，结果忽略

                        if (script->m_comboMap.find(key) ==
                            script->m_comboMap.end())
                            script->m_comboMap.insert(
                                std::make_pair(key, data));
                        // 非 [job] 分支与插入完成均继续内层循环。
                    }
                }
            }
        }
    }
    return true;
