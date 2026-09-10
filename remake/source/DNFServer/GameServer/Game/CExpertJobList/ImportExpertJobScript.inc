bool ImportExpertJobScript(STExpertJobScript* script, const char* path)
{
    // b28d-solo [P16/P292：ORIG 0x0896b102 `xor $1; test %al,%al; je` bool 形早返；局部集合照抄
    // ORIG：-0x54 scriptIdx（恒 0，成员访问索引化）/ -0x50 死 int / -0x22c 死 string /
    // -0x22d ok 预清]。
    if (!loadRDARScriptFile(g_expertJobScriptBaseDirectory, path))
        return false;
    {
        int scriptIdx = 0;
        (*script).Clear();
        std::string section;
        std::string unusedSection;   // ORIG -0x22c（构造/析构但未使用，P171 照抄）
        int unusedCount = 0;         // ORIG -0x50（死 int，P171 照抄）
        bool ok = false;             // ORIG -0x22d（预清）
        std::multimap<int, stItemSelectRate> additional;  // [additional result]
        std::multimap<int, stItemSelectRate> bigwin;      // [big win result]

        // b290-BigExpert [P164：ORIG 循环头在 body 之前（0x0896b189 = `movl $1,0x4(%esp)`
        // + ScanType + `xor $1; test; jne <body>`，且 body 尾 `jmp 896b189`），4.4 对
        // `while` 恒旋转到尾部并在末尾复制一份 cond（+6 行）⇒ 改 for(;;) + 头部早返，
        // 同 TU 的 ImportExpertJobEtcScript 已用此形拿到 189=189]。
        for (;;)
        {
            if (!ScanType(section, true))
                break;
            if (section == "[expertness exp]")
            {
                while (true)
                {
                    // b28d-solo [ORIG 0x0896b1df：先构造 ExpertJobExp，三个字段直写成员
                    // （ScanStr 直接写 e.m_name，无 string 临时+operator=）]。
                    ExpertJobExp e;
                    e.m_exp = ScanInt(&ok);
                    if (!ok) break;
                    e.m_level = ScanInt(&ok);
                    if (!ok) return false;
                    ScanStr(&e.m_name);
                    script[scriptIdx].m_expVec.push_back(e);
                }
            }
            else if (section == "[skill]")
            {
                script[scriptIdx].m_growSkillVec.clear();
                std::pair<int, int> p;
                for (;;)
                {
                    p.first = ScanInt(&ok);
                    if (!ok) break;
                    p.second = ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_growSkillVec.push_back(p);
                }
            }
            else if (section == "[additional result]")
            {
                additional.clear();
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stItemSelectRate rate;
                    rate.m_rateMin   = ScanInt(&ok); if (!ok) return false;
                    rate.m_rateMax   = ScanInt(&ok); if (!ok) return false;
                    rate.m_itemIndex = ScanInt(&ok); if (!ok) return false;
                    rate.m_rate      = ScanInt(&ok); if (!ok) return false;
                    rate.m_float8    = ScanFloat(&ok); if (!ok) return false;
                    additional.insert(std::make_pair(key, rate));
                }
            }
            else if (section == "[big win result]")
            {
                bigwin.clear();
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stItemSelectRate rate;
                    rate.m_rateMin   = ScanInt(&ok); if (!ok) return false;
                    rate.m_rateMax   = ScanInt(&ok); if (!ok) return false;
                    rate.m_itemIndex = ScanInt(&ok); if (!ok) return false;
                    rate.m_rate      = ScanInt(&ok); if (!ok) return false;
                    rate.m_float8    = ScanFloat(&ok); if (!ok) return false;
                    bigwin.insert(std::make_pair(key, rate));
                }
            }
            else if (section == "[base const]")
            {
                script[scriptIdx].m_baseConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[additional const]")
            {
                script[scriptIdx].m_additionalConst = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[disjoint result]")
            {
                script[scriptIdx].m_disjointResultMap.clear();
                while (true)
                {
                    // b290-BigExpert [P332/构造序：ORIG 0x0896b751 grade C1 之后**紧接**
                    // 0x0896b764 res C1（`lea -0x3a8(%ebp); call stDisjointResultC1Ev`），
                    // 两者均在首个 ScanInt 之前 ⇒ 原码 res 与 grade 同处循环体首并列声明。
                    // 原写法把 res 放在 grade 三字段之后 ⇒ 早退路径的析构状态机（%esi
                    // 0/1/2 三态，ORIG `mov $0x1,%esi` 12 处 vs 我方 3 处）编码不一致。
                    DisjointMachineGrade grade;
                    stDisjointResult res;
                    grade.m_grade = (unsigned char)ScanInt(&ok);
                    if (!ok) break;
                    grade.m_field1 = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    grade.m_field2 = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = ScanFloat(&ok);
                    if (!ok) return false;
                    // b290-BigExpert [P331 槽数：ORIG -0x4c(%ebp) 只有 2 次写
                    // （0x0896b8a7 / 0x0896b923）⇒ 两次 key 读复用同一具名局部，
                    // 原码是两个 int（-0x4c/-0x48）会多占一槽并整体平移其后槽位]。
                    int key;
                    key = ScanInt(&ok);
                    if (!ok) return false;
                    if (key != 0 &&
                        !expert_job::get_disjoint_additional_items(
                            additional, key, res.m_additionalItems))
                        return false;
                    key = ScanInt(&ok);
                    if (!ok) return false;
                    if (key != 0 &&
                        !expert_job::get_disjoint_additional_items(
                            bigwin, key, res.m_bigwinItems))
                        return false;
                    // ORIG 0x0896b99f `mov %eax,-0x388(%ebp)`：第三次 ScanInt 结果写入
                    // 具名局部（写后未读，P171 照抄）⇒ 必须保留该 store。
                    int readOnly = ScanInt(&ok);
                    (void)readOnly;
                    if (!ok) return false;
                    // b290-BigExpert [REAL-BUG 补：ORIG 0x0896ba7e..0x0896ba9d 在 insert
                    // 之后 `movzbl -0x2ac(%ebp); xor $1; test; je`（= 检查
                    // pair<iterator,bool>::second）⇒ 重复键走 `mov $0,%ebx; mov $1,%esi;
                    // jmp <循环尾>` = return false。原码丢弃 insert 返回值 ⇒ 重复项被静默
                    // 接受、后续 map 覆盖语义与 ORIG 不同。]
                    // ORIG 0x0896ba28 insert 的 sret 目标即具名局部 -0x2b0，
                    // 两个临时 pair 析构后（0x0896ba7e）才读 -0x2ac（= 该局部 +4 的
                    // second）⇒ 原码是「具名 pair 接收 insert 返回值，下一语句判 second」，
                    // 直接写 `if (!insert(...).second)` 会在 insert 后立即读临时。
                    std::pair<std::map<DisjointMachineGrade, stDisjointResult>::iterator,
                              bool> inserted =
                        script[scriptIdx].m_disjointResultMap.insert(
                            std::make_pair(grade, res));
                    if (!inserted.second)
                        return false;
                }
            }
            else if (section == "[clear quests]")
            {
                script[scriptIdx].m_clearQuests.clear();
                int v;
                for (;;)
                {
                    v = ScanInt(&ok);
                    if (!ok) break;
                    script[scriptIdx].m_clearQuests.push_back(v);
                }
            }
            else if (section == "[giveup cost]")
            {
                script[scriptIdx].m_giveupCost.clear();
                int v;
                for (;;)
                {
                    v = ScanInt(&ok);
                    if (!ok) break;
                    script[scriptIdx].m_giveupCost.push_back(v);
                    if (script[scriptIdx].m_giveupCost.size() > 9)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                }
            }
            else if (section == "[compound rate]")
            {
                script[scriptIdx].m_compoundRateVec.clear();
                stCompoundRate r;
                for (;;)
                {
                    r.m_level = ScanInt(&ok);
                    if (!ok) break;
                    r.m_successRate = ScanInt(&ok); if (!ok) return false;
                    r.m_expMin      = ScanInt(&ok); if (!ok) return false;
                    r.m_expMax      = ScanInt(&ok); if (!ok) return false;
                    script[scriptIdx].m_compoundRateVec.push_back(r);
                }
            }
            else if (section == "[items]")
            {
                script[scriptIdx].m_recipeInfoMap.clear();
                script[scriptIdx].m_recipeIndexMap.clear();
                script[scriptIdx].m_recipeLevelMap.clear();
                script[scriptIdx].m_recipeItemMap.clear();
                int idx;
                for (;;)
                {
                    idx = ScanInt(&ok);
                    if (!ok) break;
                    // b28d-solo [ORIG 0x0896beae..0x0896c01b：三个字段直写 info(+0/+4/+8)，
                    // 三处 make_pair 实参 = (m_field0, idx)/(m_field0, m_field8)/
                    // (m_field4, m_field0)（p167 MISSING IRmRiE/IRmRtE/IRmS0_E 实证）]。
                    stRecipeInfo info;
                    info.m_field0 = ScanInt(&ok);
                    if (!ok) return false;
                    info.m_field4 = ScanInt(&ok);
                    if (!ok) return false;
                    info.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (0xef < idx)
                    {
                        printf("SCRIPT ERROR over count [%s] %s\n",
                               path, section.c_str());
                        return false;
                    }
                    script[scriptIdx].m_recipeInfoMap.insert(std::make_pair(idx, info));
                    script[scriptIdx].m_recipeIndexMap.insert(
                        std::make_pair(info.m_field0, idx));
                    script[scriptIdx].m_recipeLevelMap.insert(
                        std::make_pair(info.m_field0, info.m_field8));
                    script[scriptIdx].m_recipeItemMap.insert(
                        std::make_pair(info.m_field4, info.m_field0));
                }
            }
            else if (section == "[endurance repair cost]")
            {
                script[scriptIdx].m_enduranceRepairCost.clear();
                stEnduranceRepairCost r;
                for (;;)
                {
                    r.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    r.m_field4 = ScanInt(&ok); if (!ok) return false;
                    script[scriptIdx].m_enduranceRepairCost.push_back(r);
                }
            }
            else if (section == "[endurance reduce]")
            {
                script[scriptIdx].m_enduranceReduce1 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_enduranceReduce2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[endurance initial value]")
            {
                script[scriptIdx].m_enduranceInit = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[upgrade cost]")
            {
                script[scriptIdx].m_upgradeCost.clear();
                int idx;
                for (;;)
                {
                    idx = ScanInt(&ok);
                    if (!ok) break;
                    // N（P321-T5）：ORIG 0x0896c268..0x0896c26e 为
                    // `mov -0x48(%ebp),%eax; sub $0x1,%eax; mov %eax,%esi`（idx 槽 +
                    // 寄存器中转、无 expect 槽，P0 判据禁止引入具名 expect）；4.4 恒折
                    // `lea -0x1(%eax),%esi`。试过具名局部形（段长对齐但新增槽、违反 P0）
                    // 后回退内联表达式，记后端指令选择差。
                    if (idx - 1 != (int)script[scriptIdx].m_upgradeCost.size())
                        return false;
                    int v = ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_upgradeCost.push_back(v);
                }
            }
            else if (section == "[potion info]")
            {
                // 读取 (itemIndex, value) 对，按 itemIndex 写入两个全局炼金术值；
                // 非法 itemIndex 返回 false。
                // b290-BigExpert [REAL-BUG 修正 + P329/switch 形态]：ORIG
                // 0x0896c35d..0x0896c398 的 switch 判定树实证 0x27ac52 落
                // `mov %eax,0x94f777c`（= g_alchemistInfo[1]），只有 0x27ac59 落
                // `mov %eax,0x94f7778`（=[0]，0x0896c3a4 独此一路）——原码把
                // 0x27ac52 误并入 [0] 组 ⇒ 该药水索引写错全局。改为 switch 形，
                // 同时复现 ORIG「条件一次取入 %eax + 二分判定树」（4.1 对 || 链
                // 发数值二分树、4.4 对 if/else-if 发线性 cmpl 链 ⇒ 24 行 struct 差）。
                for (;;)
                {
                    int idx = ScanInt(&ok);
                    if (!ok) break;
                    int val = ScanInt(&ok);
                    if (!ok) return false;
                    // ORIG 0x0896c36c 判定树用 `ja`（无符号序）⇒ switch 表达式为
                    // unsigned（P321-T2：消费者指令定符号性）。
                    // P324：叶块物理序 = 书写序。ORIG 叶块序为 [1] 组（0x0896c39a）
                    // 在前、[0]（0x0896c3a4）在后 ⇒ [1] 组必须写在前面。
                    switch ((unsigned int)idx)
                    {
                    case 0x27ac52:
                    case 0x1f8e:
                    case 0x98981a:
                    case 0x98988d:
                    case 0x989885:
                    case 0x2920913e:
                        g_alchemistInfo[1] = val;
                        break;
                    case 0x27ac59:
                        g_alchemistInfo[0] = val;
                        break;
                    default:
                        return false;
                    }
                }
            }
            else if (section == "[gain exp]")
            {
                script[scriptIdx].m_gainExp1 = ScanInt(&ok);
                if (!ok) return false;
                script[scriptIdx].m_gainExp2 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[limit disjoint gold]")
            {
                script[scriptIdx].m_limitDisjointGold = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[portable disjoint info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_portableDisjointMap.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x211,
                            "import expertjob script - [portable disjoint info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[disjoint self service]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c5ee..0x0896c5fd 于块首把
                // `&script[scriptIdx].m_selfDisjoin` 溢出到 -0x3c(%ebp)，随后 3 处
                // `mov -0x3c(%ebp),%eax` 重载 + `mov %eax,%edx; mov %edx,N(%eax)` 落值
                // （0x0896c610/0x0896c63d/0x0896c66b）。原逐点重算 imul/add 每点多 2 条。]
                STExpertJobScript::stSelfDisjoinMaterialInfo* selfDisjoin =
                    &script[scriptIdx].m_selfDisjoin;
                // N76 封顶：ORIG 0x0896c60e..0x0896c613 为 RHS-first + edx 中转
                // （`mov %eax,%edx; mov -0x3c(%ebp),%eax; mov %edx,(%eax)`），4.4 对
                // int 字段恒 LHS-first；按 P211/P248 档③试 (unsigned int)/
                // (unsigned long) 两种 RHS 异型化均被前端折叠（实测仍 LHS-first）
                // ⇒ 装载序旋钮不可用，记 N76（P321-T5：禁为装载序改类型）。
                selfDisjoin->m_field0 = ScanInt(&ok);
                if (!ok) return false;
                selfDisjoin->m_field4 = ScanInt(&ok);
                if (!ok) return false;
                selfDisjoin->m_field8 = ScanInt(&ok);
                if (!ok) return false;
            }
            else if (section == "[shop doll info]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c6ab..0x0896c6ba 块首
                // `&script[scriptIdx].m_shopDollMap` → -0x38(%ebp)，循环内 insert 只
                // `mov -0x38(%ebp),%edx` 重载（0x0896c761）]。
                std::map<int, unsigned char>* shopDollMap =
                    &script[scriptIdx].m_shopDollMap;
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned char val = (unsigned char)ScanInt(&ok);
                    if (!ok) return false;
                    if (!shopDollMap->insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x236,
                            "import expertjob script - [shop doll info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[unseal scroll level limit info]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c7d9..0x0896c7e8
                // `&script[scriptIdx].m_unsealScrollMap` → -0x34(%ebp)]。
                std::map<int, unsigned short>* unsealScrollMap =
                    &script[scriptIdx].m_unsealScrollMap;
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!unsealScrollMap->insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x24b,
                            "import expertjob script - [unseal scroll level limit info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[monstercard bind info]")
            {
                // b290-BigExpert [P332 具名指针形：ORIG 0x0896c908..0x0896c917
                // `&script[scriptIdx].m_monsterCardBindRateMap` → -0x30(%ebp)]。
                std::map<int, stMonsterCardBindRate>* monsterCardBindRateMap =
                    &script[scriptIdx].m_monsterCardBindRateMap;
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    stMonsterCardBindRate rate;
                    rate.m_field0 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    rate.m_field2 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!monsterCardBindRateMap->insert(
                            std::make_pair(key, rate)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x266,
                            "import expertjob script - [monstercard bind info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[monstercard bind only]")
            {
                int cnt[4] = { 0, 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    int idx = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_specialBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 4; ++i)
                    script[scriptIdx].m_specialBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[monstercard bind list]")
            {
                int cnt[4] = { 0, 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    int idx = ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    script[scriptIdx].m_commonBindInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 4; ++i)
                    script[scriptIdx].m_commonBindInfo.m_count[i] = cnt[i];
            }
            else if (section == "[alchemist extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_alchemistExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2b1,
                            "import expertjob script - [alchemist extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[enchanter extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_enchanterExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2c4,
                            "import expertjob script - [enchanter extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[doll_controller extraction info]")
            {
                int key;
                for (;;)
                {
                    key = ScanInt(&ok);
                    if (!ok) break;
                    unsigned short val = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    if (!script[scriptIdx].m_dollExtractInfo.insert(
                            std::make_pair(key, val)).second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2d7,
                            "import expertjob script - [doll_controller extraction info] index duplicated");
                        return false;
                    }
                }
            }
            else if (section == "[alchemist extraction result item]")
            {
                int cnt[3] = { 0, 0, 0 };
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    std::string type;
                    // b290-BigExpert [P198②：ORIG 0x0896d048 call ScanStr 之后
                    // `mov %al,-0x22d(%ebp)`（0x0896d04c）+ `movzbl; xor $1; test; je`
                    // 三句形 ⇒ 原码是「先存具名 ok 再判」，非 `if (!ScanStr(..))`；
                    // ok 此后必被下一轮 ScanInt 覆写 ⇒ 无行为差]。
                    ok = ScanStr(&type);
                    if (!ok)
                        return false;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    // ORIG 0x0896d16e `movl $0x3,-0x1c(%ebp)`：idx 具名局部初值 3
                    // （三分类各自覆盖、else 直接 return ⇒ 值 3 从不被读，P171 照抄）。
                    int idx = 3;
                    if (type == "food")
                        idx = 0;
                    else if (type == "life")
                        idx = 1;
                    else if (type == "jewel")
                        idx = 2;
                    else
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x2f9,
                            "import expertjob script - [alchemist extraction result item] result_type is invalid");
                        return false;
                    }
                    script[scriptIdx].m_alchemistResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field4;
                }
                for (int i = 0; i < 3; ++i)
                    script[scriptIdx].m_alchemistResultInfo.m_count[i] = cnt[i];
            }
            else if (section == "[enchanter extraction result item]")
            {
                int total = 0;
                while (true)
                {
                    stMonsterCardBindResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    total += (int)res.m_field4;
                    script[scriptIdx].m_enchanterResultInfo.m_vec.push_back(res);
                }
                script[scriptIdx].m_enchanterResultInfo.m_count = total;
            }
            else if (section == "[doll_controller extraction result item]")
            {
                int cnt[3] = { 0, 0, 0 };
                while (true)
                {
                    stDollControllerResult res;
                    res.m_field0 = ScanInt(&ok);
                    if (!ok) break;
                    res.m_field4 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    res.m_field6 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    // b290-BigExpert [REAL-BUG 修正]：ORIG 0x0896d48f 第 4 次 ScanInt
                    // 落 `mov %ax,-0x37c(%ebp)` = res+0x8（m_field8），非 res+0x4；
                    // 收尾 0x0896d59c `movzwl -0x37c(%ebp)` 亦取 m_field8。原码写回
                    // m_field4 ⇒ ①覆盖第 2 个值（分类条件 0x0896d4ba 读的正是 res+4）
                    // ⇒ 分派用错操作数；②入表元素的 +8 字段恒为 ctor 链式残值。
                    res.m_field8 = (unsigned short)ScanInt(&ok);
                    if (!ok) return false;
                    int idx = 3;
                    // b290-BigExpert [BigAuction 翻法三：ORIG 0x0896d4b3..0x0896d558 为
                    // 平铺 else-if 链 ⇒ 叶块物理序 = idx=0 / 1 / 2 / 报错，且每段条件
                    // 展开为「双 cmp + 反向跳转」的区间形（0x0896d4e3 cmp $0x1e jbe /
                    // 0x0896d506 cmp $0x32 jbe）。原嵌套 else 形落物理序反成
                    // 报错/2/1/0，且把 `> 0x1e` 写成 `< 0x1f` 变 `cmp $0x1f`。]
                    if (res.m_field4 != 0 && res.m_field6 <= 0x1e)
                    {
                        idx = 0;
                    }
                    else if (res.m_field4 > 0x1e && res.m_field6 <= 0x32)
                    {
                        idx = 1;
                    }
                    else if (res.m_field4 > 0x32 && res.m_field6 <= 0x50)
                    {
                        idx = 2;
                    }
                    else
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptExpertJob.cpp",
                            __FUNCTION__, 0x33b,
                            "import expertjob script - [doll_controller extraction result item] result_type is invalid");
                        return false;
                    }
                    script[scriptIdx].m_dollResultInfo.m_vec[idx].push_back(res);
                    cnt[idx] += (int)res.m_field8;
                }
                for (int i = 0; i < 3; ++i)
                    script[scriptIdx].m_dollResultInfo.m_count[i] = cnt[i];
            }
        }
        return true;
    }
    return false;
