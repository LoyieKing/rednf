// ============================================================================
// importPvpMissionScript（ORIG _Z22importPvpMissionScriptP13MissionScriptPKc，
// 0x8a5ac28）。反编译见 docs/class_func_reports/_lt_global_gt_/
// importPvpMissionScript.md。返回 false=装载/解析失败；true=成功或 EOF 正常收尾。
// ============================================================================
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
                // [P2S1PvPQuest 2026-09-11] ORIG 0x8a5b76f 是**全函数唯一**的
                //   `mov $0x1,%ebx`（`mov $1,%ebx` 计数 =1，尾 +0xb47）⇒ 所有
                //   `return true` 共享同一出口；旧文每个 return true 各发射一份
                //   （我方 `je L; mov $0x1,%ebx; jmp epi` = 3 条 vs ORIG `jne +0xb47`
                //   = 1 条，x2 处 ⇒ 正是本件 gap −2）。改共享尾标签 goto。
                goto l_end_true;                 // ORIG 扫描失败→正常收尾
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
                goto l_end_true;             // ORIG 扫描失败→正常收尾（共享唯一出口）

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
                        // [P2S1PvPQuest 2026-09-11] ORIG 0x8a5b2f3 `call
                        //   _ZNSt4pairIiiEC1Ev`（默认构造临时）紧接 0x8a5b316
                        //   `call _ZNSt6vectorISt4pairIiiESaIS1_EE6resizeEjS1_`
                        //   （4.4 的 resize 第二参**按值**）= `resize(n)` 单参形
                        //   （默认实参 value_type()），无具名 fill 局部 ⇒ 我方
                        //   旧文具名 fill 多占 8B 槽/帧。填充值随即被下方 for
                        //   逐元素覆盖，语义等价。
                        data.m_rewardList.resize(n);
                        for (int i = 0; i < n; ++i)
                        {
                            data.m_rewardList[i].first = ScanInt((bool*)0);
                            data.m_rewardList[i].second = ScanInt((bool*)0);
                        }

                        if (!ScanType(comboTok, true))
                            continue;        // ORIG esi=1 → 循环头再读一次

                        if ("[reward item]" == comboTok)
                        {
                            // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x08a5b3c0/0x08a5b3d2：两次
                            //   ScanInt 结果 `mov %eax,-0x88(%ebp)` / `mov %eax,-0x84(%ebp)`，
                            //   即 data(-0x94，见 0x08a5b30d resize 的 this) 的 +0x0c/+0x10
                            //   = m_bonus.first/second，随后 0x08a5b458 make_pair(key,data)
                            //   插入 m_comboMap。旧注释「读入后未使用」误判：丢弃后
                            //   CMission_combo_clear::_Reward 发放的 m_bonus 恒为 (0,0)。
                            data.m_bonus.first = ScanInt((bool*)0);
                            data.m_bonus.second = ScanInt((bool*)0);
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
l_end_true:
    return true;
}