bool importServerParameterScript(ServerParameterScript* script_arg,
                                 const char* path)
{
    // b28d-solo-ServerParameterScript: DIFF（N82+N89+N91 封顶；p167 差集已大幅收敛）
    // 本波 p167 定位并修正：① [escalade tutorial reward] operator[] → find/end/ne/
    // ptEv + 两分支 push_back + insert；② [training first give] m_map49c4[a]=1 →
    // insert；③ int,int make_pair → std::pair<int,int>(a,b)（9 点：m_vecMapped/
    // m_vecShareKind/m_vec0/m_vecc/m_vec18(blood)/ontime/vec/escalade×2）；④ [ontime
    // event reward] pair 直构（该条原写的 break→return 0 系误改，见下方 TakeSrvParam
    // 复核：ORIG 岛落点证 break，已回改）；⑤ [drop upgrade
    // rarity] rarity int→ENUM_RARITY（含 CItemAmplifier.h include）；⑥ [drop bonus of
    // dungeon level increase] 具名 pair 左值 push_back；⑦ [bring up event] `line !=`
    // → 正向 `line ==`；⑧ m_levels(×2)/m_vec6c push_back 实参内联（push_backEOi）；
    // ⑨ m_pvp49ac make_pair 第一参改匿名右值；⑩ [full level reward] 去 (unsigned) cast。
    // ⑪ [ontime event reward] `int a=0; unsigned short b=0;` 预清两句式 + pair 默认构造
    // + 逐字段写 + 左值 push_back；⑫ [pvp live event reward] 同形。
    // **p167 终态：MISSING=0**（全部语义调用点已还原）；EXTRA 仅 ①空析构 D1 ×2/类型
    // 共 16（4.4 对局部空析构发射 D1 调用，ORIG 4.1 内联省略；D1 W 符号必须保留
    // ⇒ N91 封顶）。
    // ndiff 主体为 N82（帧/槽复用）与 N89（STL out-of-line 实例）。
    // [接手复核 TakeSrvParam 2026-09-09，p167_callsite.py 修正版（旧版在 stripped
    // ORIG 上有假 CLEAR）] 派单假设「我方把若干段合并 ⇒ ORIG-only 游戏 callee 非空」
    // 经修正版工具证伪：ORIG 558 / OUR 574 calls，差集仅 C2↔C1、D2↔D1 代际命名
    // （附二 §2 豁免），归一后 = 0。派单点名项逐项实测：ScanFloat 23/23（ORIG
    // 0x8a8723f…0x8a8c145 与我方 0x2e2a…0x7e61 地址间隔同构，无合并/改形）、
    // GetEquipmentTypeStringToEnum 1/1、logFormat 2/2、stServerIndex 10/10、
    // sItemGenRef 6/6。分支标签集：ORIG .rodata 137 项 vs 本源码 137 项全覆盖
    // （0 缺段）。[三口径并列] 修正版 p167 MISSING=0；旧版 p167_callsite_legacy
    // 复跑 MISSING 亦为空、EXTRA 仅 8×D1；call_multiset_diff TOT 558/574、
    // diffkeys 8 全为 D1 ⇒ 派单「ORIG-only callee」清单出自更粗聚合口径，逐项皆假。
    // gap = 4615 − 4742 = −127（我方多）：+16 call 全为局部空析构 D1
    // （N91），帧 0x54c vs 0x55c（N82）；fstps 23/23。⇒ guide §6.2 判「语义完成、
    // byte 层 N 封顶」。
    // [TakeSrvParam 逐段复核 2026-09-09：call 锚点间隔 + 136 个 [tag] 块段级
    //  归一化多重集] 上述「N 封顶」经重核后仍有**真语义缺口 1 类**并已修：
    //  · [ontime event reward] 旧译「break→return 0」系误改：ORIG 两处失败落点
    //    0x08a89fe5/0x08a8a009 均为 `jne 08a8c1a0/08a8c1a3`（岛 `nop;jmp +0x5117`
    //    = 外层循环继续）⇒ 原码是 break；同块 b 消费端 0x08a8a026 `movzwl` 零扩展
    //    ⇒ 真型 unsigned short（旧 short 生 movswl，值 ≥0x8000 时分叉）。已改
    //    break + unsigned short（该块段级多重集归零）。
    //  · [protect of upgrade down jpn] / [protect of upgrade down]：旧译用
    //    `char* pb = (char*)&p` 间接写（P183 禁指针缓存）+ key 无预清；ORIG
    //    0x08a8b665/0x08a8b7d0 `movl $0,-0x4c4/-0x4cc` 两句式 + 直接写 m_view 字段。
    //    已改（两块归零）。
    //  · 另修 16 个 [tag] 块的 P198① 两句式预清 / 声明位置（ORIG 有 `movl $0,-N`
    //    而我方缺）：server message index onoff/kind、server message event kind、
    //    price average、admission increase、lotto point、full level reward、
    //    pvp exp penalty revision、unusable/usable items、training seal give、
    //    pvp channel grade penalty、drop upgrade rarity（含 ItemFarmingData data
    //    声明前移至 ScanStr 守卫之前，P334）、escalade tutorial reward、
    //    luck point deduction gap（含 -0xa4 死写入 P171 + -0xa0 槽复用 P160）、
    //    assault chaos point weight（0x08a88826 死写入 $1）。
    //  · 复核后 136 块中仅 26 块仍有差异，全部为 N 类：4.4 DW2 EH landing pad
    //    （8 类空析构 ×2 = 16 call，N91）、P211 store 装载序（[upgrade with legacy]
    //    9 处 RHS-first；翻正需改共享头 m_508[20] int→unsigned 并跨 TU 联判
    //    W8_CU_A.cpp 消费点，本泳道未动）、add-vs-lea 寻址（N4/N102）。
    //  · p167 归一化后 MISSING=0（复核复跑）；gap = 4534 − 4689 = −155（补 16 块
    //    预清后我方指令数 +16，属结构对齐代价）；TU 四数
    //    identical=620/ae=1136/near=4/diff=9 前后不变（未回退）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    // [BigSrvParam2] ORIG prologue（0x08a870e7-0x08a870f6）：load 成功后把形参
    // script 一次性写入本地槽 -0x11c，此后 226 处成员访问全部 `mov -0x11c(%ebp)`
    // 重读 ⇒ 原码有本地指针副本。改为 script_arg 形参 + 局部 script。
    // [BigQuest 探针记录] 试过 `ServerParameterScript* volatile script`（P332
    // ImportDungeonScript 配方）强制逐点重载：ndiff 3834→4327 **反劣** —— 本件
    // ORIG 副本槽=-0x11c / missed=-0x115 / line=-0x370，我方=…-0x34c/-0x11d/-0x350，
    // 帧错位未解前加 reload 只增行不命中 ⇒ 回滚为普通副本。属 N82（帧分配）族。
    ServerParameterScript* script = script_arg;
    script->clear();
    std::string line;
    bool ok = false;
    bool missed = false;
    // [G19 P168a 试形] ORIG 该扫描循环头为 `call ScanType; xor $0x1,%eax; test %al,%al;
    // jne <exit>`（bool callee 裸取反）⇒ 源形须写 for(;;)+if(!ScanType) break；
    // `while(ScanType(...))` 在 4.4 直产 `test %al; jne`（少 xor、块位置亦不同）。
    // 依据 = DiffG21.G21F 实证（importSecretShopScript 644/640 gap+4 → 644/644 gap+0）。
    for (;;)
    {
        if (!ScanType(line, true))
            break;
        if (line == "[nation code]")
        {
            ScanInt((bool*)0);
        }
        else if (line == "[script version]")
        {
            script->m_358 = (unsigned int)ScanInt((bool*)0);
            g_new_scriptVersion = script->m_358;
        }
        else if (line == "[version]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_versionBytes[i] = (unsigned char)ScanInt((bool*)0);
            }
        }
        else if (line == "[clear exp bonusrate]")
        {
            script->m_expRate360[0] = ScanFloat((bool*)0);
        }
        else if (line == "[monster exp bonusrate]")
        {
            script->m_expRate360[1] = ScanFloat((bool*)0);
        }
        else if (line == "[party user number exp bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup368[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number exp bonusrate starter server]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup378[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[quest item drop bonus starter server]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup388[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[dungeon difficulty exp bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_monsterExpTable398[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[clear rank exp bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_expBonusWeight3ac[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[drop prob number]")
        {
            sItemGenRef zero = {0, 0, 0, 0, 0};
            script->m_genRefs0.resize(
                (unsigned int)ScanInt((bool*)0), zero);
        }
        else if (line == "[drop prob]")
        {
            for (int i = 0; i < (int)script->m_genRefs0.size(); ++i)
            {
                sItemGenRef& r = script->m_genRefs0[i];
                r.m_field0 = ScanInt((bool*)0);
                r.m_field4 = ScanInt((bool*)0);
                r.m_field8 = ScanInt((bool*)0);
                // ORIG 0x08a875b7 实证：第 4 个 ScanInt 写入 +0x10（m_dropRate），
                // +0x0c 固定 1000（SetItemDropRate 缩放 +0x10）。
                r.m_dropRate = ScanInt((bool*)0);
                r.m_fieldc = 1000;
            }
        }
        else if (line == "[dungeon difficulty drop bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_floatPool3cc[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number drop bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_floatPool3e0[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[drop bonusrate of monster kind]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_flagExpRate3f0[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[result reward prob number]")
        {
            sItemGenRef zero = {0, 0, 0, 0, 0};
            script->m_genRefs1.resize(
                (unsigned int)ScanInt((bool*)0), zero);
        }
        else if (line == "[result reward prob]")
        {
            for (int i = 0; i < (int)script->m_genRefs1.size(); ++i)
            {
                sItemGenRef& r = script->m_genRefs1[i];
                r.m_field0 = ScanInt((bool*)0);
                r.m_field4 = ScanInt((bool*)0);
                r.m_field8 = 0;
                // ORIG 0x08a8786a 实证：第 4 个 ScanInt 写入 +0x10（m_dropRate），
                // +0x0c 固定 1000（与 [drop prob] 节一致）。
                r.m_dropRate = ScanInt((bool*)0);
                r.m_fieldc = 1000;
            }
        }
        else if (line == "[dungeon difficulty reward bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_floatPool40c[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number reward bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_floatPool420[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[basis of item dicision]")
        {
            script->m_430 = ScanInt((bool*)0);
        }
        else if (line == "[basis of equipment rarity dicision]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_ints434[i] = ScanInt((bool*)0);
            }
        }
        else if (line == "[basis of stackable rarity dicision]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_ints444[i] = ScanInt((bool*)0);
            }
        }
        else if (line == "[stamina recovery cost]")
        {
            script->m_vec57c.clear();
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec57c.push_back(v);
            }
        }
        else if (line == "[increase abuse point]")
        {
            script->m_increaseAbusePoint = ScanInt(&ok);
        }
        else if (line == "[decrease abuse point]")
        {
            script->m_decreaseAbusePoint = ScanInt(&ok);
        }
        else if (line == "[abuse point penalty]")
        {
            script->m_abusePointPenalty = ScanInt(&ok);
        }
        else if (line == "[decrease experience point]")
        {
            script->m_decreaseExperiencePoint = ScanInt(&ok);
        }
        else if (line == "[upgrade with legacy]")
        {
            script->m_508[8] = ScanInt((bool*)0);
            script->m_508[9] = ScanInt((bool*)0);
            script->m_508[10] = ScanInt((bool*)0);
            script->m_508[11] = (script->m_508[9] - script->m_508[8]) * 1000 + 999;
            script->m_508[12] = ScanInt((bool*)0);
            script->m_508[13] = ScanInt((bool*)0);
            script->m_508[14] = ScanInt((bool*)0);
            script->m_508[15] = (script->m_508[13] - script->m_508[12]) * 1000 + 999;
            script->m_508[16] = ScanInt((bool*)0);
            script->m_508[17] = ScanInt((bool*)0);
            script->m_508[18] = ScanInt((bool*)0);
            script->m_508[19] = (script->m_508[17] - script->m_508[16]) * 1000 + 999;
        }
        else if (line == "[notice legacy with rarity]")
        {
            bool bad;
            int v = ScanInt(&bad);
            if (!bad)
            {
                LogManager::logFormat(0, "../RDARScriptServerParameter.cpp",
                                      __FUNCTION__, 400,
                                      "importUpgradeTable - invalid notice legacy with rarity tag");
            }
            script->m_558 = v;
        }
        else if (line == "[notice legacy with type]")
        {
            memset(script->m_55c, 0, sizeof(script->m_55c));
            std::string s;
            while (ScanStr(&s))
            {
                int idx = GetEquipmentTypeStringToEnum(s);
                script->m_55c[idx] = 1;
            }
        }
        else if (line == "[notice legacy with upgrade]")
        {
            bool bad;
            int v = ScanInt(&bad);
            if (!bad)
            {
                LogManager::logFormat(0, "../RDARScriptServerParameter.cpp",
                                      __FUNCTION__, 0x1a7,
                                      "importUpgradeTable - invalid notice legacy with upgrade tag");
            }
            script->m_noticeLegacyWithUpgrade = v;
        }
        else if (line == "[collect dungeon statistic server]")
        {
            while (true)
            {
                stServerIndex key;
                key.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                key.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map0.insert(std::make_pair(key, true));
            }
        }
        else if (line == "[light server no]")
        {
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_map24.insert(std::make_pair(v, true));
            }
        }
        else if (line == "[burning fatigue event param]")
        {
            while (true)
            {
                stBurningFatigueParam_t p;
                p.m_level = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_bonusExpRate = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_refillFatigue = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field10 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field14 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field0c = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[0] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[3] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_burning48c.push_back(p);
            }
        }
        else if (line == "[fatigue event param]")
        {
            while (true)
            {
                stFatigueEventParam_t p;
                p.m_field0 = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field8 = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field9 = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_fielda = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_fieldb = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_fatigue4a8.push_back(p);
            }
        }
        else if (line == "[light server notice]")
        {
            while (true)
            {
                stLightServerNotice_t n;
                n.m_hour = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                n.m_min = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = ScanStr(&n.m_message);
                if (!ok)
                {
                    return 0;
                }
                script->m_light3c.push_back(n);
            }
        }
        else if (line == "[assault cost]")
        {
            script->m_vec588.clear();
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec588.push_back(v);
            }
        }
        else if (line == "[assault channel]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: ORIG 此段仍为 make_pairIRiS0_
                // （pair 直构仅命中 escalade/m_vecMapped/m_vecShareKind/m_vec0/
                // m_vecc/m_vec18(blood)/m_vec4880/vec 共 9 点），回退。
                script->m_vec18.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[assault user runaway notice]")
        {
            ok = ScanStr(&script->m_str594);
            if (!ok)
            {
                return 0;
            }
        }
        else if (line == "[assault term panalty]")
        {
            script->m_598 = ScanInt((bool*)0);
        }
        else if (line == "[assault term panalty keep up time]")
        {
            script->m_59c = ScanInt((bool*)0);
        }
        else if (line == "[assault level panalty]")
        {
            script->m_assaultLevelPenalty = ScanFloat((bool*)0);
        }
        else if (line == "[assault chaos point weight]")
        {
            script->m_float5a4.clear();
            // ORIG 0x08a88826 `movl $0x1,-0xc4(%ebp)`：块内无读取点（P171 死写入
            // 照抄）；两句式使 4.4 保留 store（P198 孪生）。
            int deadWeightFlag;
            deadWeightFlag = 1;
            (void)deadWeightFlag;
            while (true)
            {
                float v = ScanFloat(&ok);
                if (!ok)
                {
                    break;
                }
                if (script->m_float5a4.size() == 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        script->m_float5a4.push_back(v);
                    }
                }
                else
                {
                    for (int i = 0; i < 5; ++i)
                    {
                        script->m_float5a4.push_back(v);
                        if (script->m_float5a4.size() > 99)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if (line == "[luck point]")
        {
            while (true)
            {
                int unused = 0;
                unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                stLuckPoint_t p;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field1 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_luck48.push_back(p);
            }
        }
        else if (line == "[luck point deduction rare]")
        {
            while (true)
            {
                stLuckPointDeduction_t p;
                int level = 0;
                level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_levels.reserve(4);
                for (int i = 0; i < 4; ++i)
                {
                    // b28d-solo-ServerParameterScript: ORIG 0x08a88ad0 段
                    // `call ScanInt; mov %eax,-0x2fc; lea -0x2fc; call push_backEOi`
                    // ⇒ 实参内联（右值）而非具名 v（p167 MISSING push_backEOi ×3 /
                    // EXTRA push_backERKi ×3）。
                    p.m_levels.push_back(ScanInt(&ok));
                    if (!ok)
                    {
                        return 0;
                    }
                }
                (void)level;
                script->m_luckDed54.push_back(p);
            }
        }
        else if (line == "[luck point deduction unique]")
        {
            while (true)
            {
                stLuckPointDeduction_t p;
                int level = 0;
                level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_levels.reserve(4);
                for (int i = 0; i < 4; ++i)
                {
                    // b28d-solo-ServerParameterScript: 同 rare 段（push_backEOi）。
                    p.m_levels.push_back(ScanInt(&ok));
                    if (!ok)
                    {
                        return 0;
                    }
                }
                (void)level;
                script->m_luckDed60.push_back(p);
            }
        }
        else if (line == "[luck point deduction gap]")
        {
            // ORIG 0x08a88cf3 `movl $0,-0xa4`（预清后块内无读取点，P171 死写入
            // 照抄；P232 未使用局部仍占槽）。
            int deadGap = 0;
            (void)deadGap;
            script->m_vec6c.reserve(4);
            for (int i = 0; i < 3; ++i)
            {
                // ORIG 0x08a88d22 `movl $0,-0x98` 两句式预清（P198①）。
                int unused = 0;
                unused = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                // b28d-solo-ServerParameterScript: ORIG 0x08a88d80 段
                // `call ScanInt; mov %eax,-0x2f4; lea -0x2f4; call push_backEOi`
                // ⇒ 实参内联（push_backEOi）。
                script->m_vec6c.push_back(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
            }
            // ORIG 0x08a88db1/0x08a88df0 两次 ScanInt 复用同一槽 -0xa0
            // （P160 复用 ORIG 复用的槽）⇒ 单一具名局部两次赋值。
            int unused2 = 0;
            unused2 = ScanInt(&ok);
            if (!ok)
            {
                return 0;
            }
            unused2 = ScanInt(&ok);
            if (!ok)
            {
                return 0;
            }
        }
        else if (line == "[point per fatigue]")
        {
            script->m_78 = ScanInt((bool*)0);
        }
        else if (line == "[event point per fatigue]")
        {
            script->m_7c = ScanInt((bool*)0);
        }
        else if (line == "[point per play time]")
        {
            script->m_80 = ScanInt((bool*)0);
            script->m_84 = ScanInt((bool*)0);
        }
        else if (line == "[black list help parameter]")
        {
            script->m_blackListHelpParameter0 = ScanInt((bool*)0);
            script->m_blackListHelpParameter1 = ScanInt((bool*)0);
        }
        else if (line == "[unify server prefix]")
        {
            script->m_preventPrefixCount = ScanInt((bool*)0);
            for (int i = 0; i < script->m_preventPrefixCount; ++i)
            {
                ScanStr(script->m_preventPrefixEntries + i * 0x1e, 0x1e);
            }
        }
        else if (line == "[damaged equipment ratio]")
        {
            script->m_49e8 = ScanInt((bool*)0);
        }
        else if (line == "[limit fatigue battery charging amount]")
        {
            script->m_fatigueTable5c0.m_capLimit =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[end level fatigue battery weight]")
        {
            script->m_fatigueTable5c0.m_chargeRate4 = ScanFloat((bool*)0);
        }
        else if (line == "[character level fatigue battery constance]")
        {
            script->m_fatigueTable5c0.m_chargeDivisor8 =
                (unsigned char)ScanInt((bool*)0);
        }
        else if (line == "[limit fatigue battery level]")
        {
            script->m_fatigueTable5c0.m_minChargeLevel9 =
                (unsigned char)ScanInt((bool*)0);
        }
        else if (line == "[fatigue battery money amount]")
        {
            script->m_fatigueTable5c0.m_fieldc = ScanInt((bool*)0);
        }
        else if (line == "[fatigue battery level for money active]")
        {
            script->m_fatigueTable5c0.m_endLevel10 =
                (unsigned char)ScanInt((bool*)0);
        }
        else if (line == "[min fatigue battery per buff active]")
        {
            script->m_fatigueTable5c0.m_batteryThreshold1a8 =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[limit grownup buff cnt fatigue battery]")
        {
            int i = 0;
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                *reinterpret_cast<short*>(
                    script->m_fatigueTable5c0.m_data11 + 1 + i * 2) =
                    (short)v;
                ++i;
            }
        }
        else if (line == "[fatigue battery per grownup buff active]")
        {
            script->m_fatigueTable5c0.m_activeSwitch1a4 =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[grownup buff per fatigue battery active]")
        {
            script->m_fatigueTable5c0.m_bonusMultiplier1a6 =
                (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[pc room party exp bonus rate]")
        {
            float v = ScanFloat(&ok);
            if (!ok)
            {
                return 0;
            }
            script->m_76c = v;
        }
        else if (line == "[escalade tutorial reward]")
        {
            while (true)
            {
                // ORIG 0x08a892b0 `movl $0,-0x3f8` 两句式预清（P198①）；a/b
                // （-0x3fc/-0x400）ORIG 无预清，保持初始化式。
                int key = 0;
                key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: P123 查找形式修正 — ORIG 0x08a8d1xx 段
                // 为 find/end/ne/ptEv + 两分支 push_backEOS1_ + else 内 vec C1Ev +
                // make_pair(IRi,RSt6vector) + pair<const int,vector> 转换 + insertERKS8_；
                // 旧 `m_map88[key]` 走 operator[]（p167 EXTRA ixERS7_ +1 / MISSING
                // find/end/ne/ptEv/insert 各 1）。
                std::map<int, std::vector<std::pair<int, int> > >::iterator it =
                    script->m_map88.find(key);
                if (it != script->m_map88.end())
                {
                    it->second.push_back(std::pair<int, int>(a, b));
                }
                else
                {
                    std::vector<std::pair<int, int> > vec;
                    vec.push_back(std::pair<int, int>(a, b));
                    script->m_map88.insert(std::make_pair(key, vec));
                }
            }
        }
        else if (line == "[server message index onoff]")
        {
            while (true)
            {
                // ORIG 0x08a89547/0x08a89551 `movl $0,-0x414/-0x84` 两句式预清
                // （P198① + P212 全头部声明）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (b == 1)
                {
                    script->m_shared770.m_vecIndex.push_back(a);
                }
            }
        }
        else if (line == "[server message index kind]")
        {
            while (true)
            {
                // ORIG 0x08a895f1/0x08a895fb 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_shared770.m_vecMapped.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[server message event kind]")
        {
            while (true)
            {
                // ORIG 0x08a896b4/0x08a896be 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_shared770.m_vecShareKind.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[price average]")
        {
            while (true)
            {
                // ORIG 0x08a89777/0x08a89781 两句式预清（P198①）；首个 ScanInt
                // （0x08a89794）返回值无 store ⇒ 原码直接丢弃返回值，无具名 unused。
                int a = 0;
                int b = 0;
                ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vec0.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[lotto cost]")
        {
            script->m_7b8 = ScanInt((bool*)0);
        }
        else if (line == "[jackpot]")
        {
            script->m_7bc = ScanInt((bool*)0);
        }
        else if (line == "[admission increase]")
        {
            while (true)
            {
                // ORIG 0x08a898d6/0x08a898e0 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vecc.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[lotto point]")
        {
            while (true)
            {
                // ORIG 0x08a899a1/0x08a899ab 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vec18.push_back(std::pair<int, int>(a, b));
            }
        }
        else if (line == "[emblem compound info]")
        {
            while (true)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_emblemCompound[idx].m_cost = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                for (int j = 0; j < 5; ++j)
                {
                    script->m_emblemCompound[idx].m_result[j] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                }
            }
        }
        else if (line == "[recipe upgrade table]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe amplify table]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr1868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe upgrade table 2nd]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr2868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe amplify table 2nd]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    script->m_arr3868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[upper bind cube rate bonus]")
        {
            script->m_49ec = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[lower bind cube rate bonus]")
        {
            script->m_49f0 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[full level reward]")
        {
            while (true)
            {
                // ORIG 0x08a89ec8/0x08a89ed2 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: ORIG pair C1 模板参为 <int,int>
                // （_ZNSt4pairIKijEC1IiiEEOS_IT_T0_），旧 (unsigned int) cast 使实例
                // 变 <int,unsigned>（p167 EXTRA C1IijE 1 / MISSING C1IiiE 1）。
                script->m_map4868.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[ontime event reward]")
        {
            while (true)
            {
                // b28d-solo-ServerParameterScript: ORIG 0x08a89f60 段逐条
                // `movl $0,-0x48` + `movw $0,-0x42`（P198① 声明预清两句式）→
                // a=ScanInt（int 槽）/b=(unsigned short)ScanInt（mov %ax 16 位槽；
                // 消费端 ORIG 0x08a8a026 `movzwl` 零扩展 ⇒ T1 直证真型 unsigned short）→
                // `pair C1Ev` 默认构造 + p.first/p.second 逐字段写 → push_backERKS1_。
                // 失败落点：ORIG 0x08a89fe5/0x08a8a009 `jne 08a8c1a0/08a8c1a3`（岛
                // `nop;jmp +0x5117` = 外层循环继续）⇒ 原码是 break，非 return 0。
                int a = 0;
                unsigned short b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::pair<int, int> p;
                p.first = a;
                p.second = b;
                script->m_vec4880.push_back(p);
            }
        }
        else if (line == "[pvp live event play count]")
        {
            script->m_ac = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[pvp live event reward]")
        {
            while (true)
            {
                // b28d-solo-ServerParameterScript: ORIG 0x08a8a0c0 段
                // `pair C1Ev`（默认构造）→ p.first=ScanInt / p.second=ScanInt →
                // `push_backERKS1_`（左值）；p167 MISSING C1Ev 2 + ERKS1_ 2。
                std::pair<int, int> p;
                p.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.second = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_veca0.push_back(p);
            }
        }
        else if (line == "[pvp exp penalty revision]")
        {
            while (true)
            {
                // ORIG 0x08a8a15f/0x08a8a169 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_mapb0.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[unusable equipment info]")
        {
            while (true)
            {
                std::string s;
                ok = ScanStr(&s);
                if (!ok)
                {
                    break;
                }
                stUnusableEquipmentInfo info;
                if (s == "relic")
                {
                    info.m_field0 = 2;
                }
                else if (s == "unique")
                {
                    info.m_field0 = 3;
                }
                else if (s == "choronicle")
                {
                    info.m_field0 = 4;
                }
                else if (s == "epic")
                {
                    info.m_field0 = 5;
                }
                else if (s == "rare_avatar")
                {
                    info.m_field0 = 6;
                }
                else if (s == "common")
                {
                    info.m_field0 = 7;
                }
                else if (s == "uncommon")
                {
                    info.m_field0 = 8;
                }
                else if (s == "rare")
                {
                    info.m_field0 = 9;
                }
                else if (s == "rare_creature")
                {
                    info.m_field0 = 10;
                }
                else if (s == "real_rare")
                {
                    info.m_field0 = 1;
                }
                else
                {
                    return 0;
                }
                info.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                info.m_field8 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_unusableEC.push_back(info);
            }
        }
        else if (line == "[daily match count]")
        {
            script->m_f8 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[unusable items]")
        {
            while (true)
            {
                // ORIG 0x08a8a551 两句式预清（P198①）。
                int v = 0;
                v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_unusableItemFC.insert((unsigned long)v);
            }
        }
        else if (line == "[usable items]")
        {
            while (true)
            {
                // ORIG 0x08a8a5d1/0x08a8a5d8 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map114[(ENUM_EQUIPMENTTYPE)a].insert((unsigned long)b);
            }
        }
        else if (line == "[dungeon inout time]")
        {
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                STDungeonInoutTime t;
                t.m_startHour = ScanInt(&ok);
                t.m_endHour = ScanInt(&ok);
                t.m_startMin = ScanInt(&ok);
                t.m_endMin = ScanInt(&ok);
                if (t.m_endHour < t.m_startHour || t.m_endMin < t.m_startMin ||
                    t.m_endHour == 0x18 || t.m_endMin == 0x3c)
                {
                    return 0;
                }
                script->m_dungeonInoutc8.insert(std::make_pair(key, t));
            }
        }
        else if (line == "[member bonus fatigue limit]")
        {
            script->m_488c = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_4890 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[premiun card cost]")
        {
            script->m_premiumGoldCard4894.m_cost = ScanFloat(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[premium card drop]")
        {
            int i = 0;
            while (true)
            {
                script->m_premiumGoldCard4894.m_conds[i].m_field0 =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_premiumGoldCard4894.m_conds[i].m_field4 =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_premiumGoldCard4894.m_conds[i].m_field8 =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_premiumGoldCard4894.m_conds[i].m_fieldc =
                    ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ++i;
                if (9 < i)
                {
                    break;
                }
            }
            script->m_premiumGoldCard4894.m_conditionCount = (short)i;
        }
        else if (line == "[premium rarity dicision]")
        {
            script->m_premiumGoldCard4894.m_rarityDecision[0] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[1] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[2] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[3] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[4] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_premiumGoldCard4894.m_rarityDecision[5] =
                ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[max clean chatting count]")
        {
            script->m_4960 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[clean chatting count]")
        {
            while (true)
            {
                int a = 0;
                int b = 0;
                ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_vec4954.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[clean chatting duplication]")
        {
            script->m_4964 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[max monster drop per map]")
        {
            script->m_4968 = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[max monster count per map]")
        {
            script->m_496c = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[private training seal gain count]")
        {
            script->m_schoolMatchSeal[0] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[1] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[2] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[3] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[relay battle training seal gain count]")
        {
            script->m_schoolMatchSeal[4] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[5] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[6] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
            script->m_schoolMatchSeal[7] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[training seal bonus count]")
        {
            script->m_schoolMatchDailyPlayLimit =
                (unsigned short)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[training seal give]")
        {
            while (true)
            {
                // ORIG 0x08a8af2f/0x08a8af39 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map4994.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[pvp channel grade penalty]")
        {
            while (true)
            {
                // ORIG 0x08a8b022/0x08a8b02c 两句式预清（P198①）。
                int a = 0;
                int b = 0;
                a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map497c.insert(std::make_pair(a, b));
            }
        }
        else if (line == "[training first give]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int c = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                int n = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                std::vector<std::pair<int, int> > vec;
                for (int i = 0; i < n; ++i)
                {
                    int x = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int y = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    vec.push_back(std::pair<int, int>(x, y));
                }
                // b28d-solo-ServerParameterScript: ORIG 0x08a8d5bc
                // `_ZSt9make_pairIN21…PvPPlayerKeyE…`（第一参**按值**）⇒ 实参为匿名
                // 右值临时（p167 EXTRA make_pairIRN… 1 / MISSING make_pairIN… 1）。
                script->m_pvp49ac.insert(std::make_pair(
                    ServerParameterScript::PvPPlayerKey((unsigned char)a,
                                                        (unsigned char)b,
                                                        (unsigned char)c),
                    vec));
                std::map<int, int>::iterator it = script->m_map49c4.find(a);
                // b28d-solo-ServerParameterScript: P123 — ORIG 用 ne(find,end) 正极性 +
                // insertERKS4_（0x08a8b3ec/0x8a8b468 段），旧 `m_map49c4[a]=1` 走
                // operator[]（p167 EXTRA ixERS3_ +1 / MISSING insertERKS4_ 1）。
                if (it != script->m_map49c4.end())
                {
                    ++it->second;
                }
                else
                {
                    script->m_map49c4.insert(std::make_pair(a, 1));
                }
            }
        }
        else if (line == "[pvp item give]")
        {
            while (true)
            {
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec49dc.push_back(v);
            }
        }
        else if (line == "[bring up event]")
        {
            if (!ScanType(line, true))
            {
                goto lab_finish;
            }
            if (line == "[get eventitem fatigue]")
            {
                while (true)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    script->m_eventIdolBringUp454.m_vec8.push_back(v);
                }
            }
            if (!ScanType(line, true))
            {
                goto lab_finish;
            }
            // b28d-solo-ServerParameterScript: ORIG 全函数只用 eq（_ZSteqIcSt11char…）
            // ⇒ 原码为正向 `if (line == …)`（p167 EXTRA _ZStneIc… 1 / MISSING eq 1）。
            if (line == "[get reward count]")
            {
                script->m_eventIdolBringUp454.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    goto lab_finish;
                }
                script->m_eventIdolBringUp454.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    goto lab_finish;
                }
            }
        }
        else if (line == "[protect of upgrade down jpn]")
        {
            while (true)
            {
                // ORIG 0x08a8b665 `movl $0,-0x4c4` + 0x08a8b66f ScanInt ⇒ key 预清
                // 两句式（P198①）。p 的四个字节 ORIG 直接写槽 -0x4c0..-0x4bd
                // （P183 禁指针缓存 ⇒ 用具名 m_view 字段，勿 char* pb 间接写）。
                unsigned int key = 0;
                key = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stUpgradeItemTicketParam_t p;
                p.m_view.m_minUpgrade = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_view.m_maxUpgrade = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_view.m_ticketFlag = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                int f4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (0 < f4)
                {
                    p.m_view.m_field3 = 1;
                }
                script->m_ticket468.insert(std::make_pair(key, p));
            }
        }
        else if (line == "[protect of upgrade down]")
        {
            while (true)
            {
                // ORIG 0x08a8b7d0 `movl $0,-0x4cc` + 0x08a8b7da ScanInt ⇒ key 预清
                // 两句式（P198①）；p 的字节 ORIG 直接写槽 -0x4c8..-0x4c6
                // （P183 禁指针缓存 ⇒ 具名 m_view 字段）。
                unsigned int key = 0;
                key = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stUpgradeItemTicketParam_t p;
                p.m_view.m_minUpgrade = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_view.m_maxUpgrade = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_view.m_ticketFlag = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_ticket468.insert(std::make_pair(key, p));
            }
        }
        else if (line == "[weekend bonus fatigue]")
        {
            script->m_weekendBouns4b4.m_bonusFatiguePercent =
                (unsigned short)ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[weekend bonus exp]")
        {
            script->m_weekendBouns4b4.m_bonusExpPercent = ScanInt(&ok);
            if (!ok)
            {
                goto lab_finish;
            }
        }
        else if (line == "[drop routin by party]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_weekendBouns4b4.m_dropRoutineByParty[i] =
                    (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
            }
        }
        else if (line == "[drop upgrade rarity]")
        {
            while (true)
            {
                std::string s;
                // ORIG 0x08a8ba08 SsC1Ev → 0x08a8ba16 ItemFarmingDataC1Ev →
                // 0x08a8ba1b/0x08a8ba25 itemId/weight 两句式预清 → 0x08a8ba2f
                // rarity=6，全部位于 0x08a8ba42 ScanStr 之前（P212 全头部声明 +
                // P334 守卫前构造：ScanStr 失败 break 路径 ORIG 仍析构 data）。
                ItemFarmingData data;
                int itemId = 0;
                int weight = 0;
                ENUM_RARITY rarity = (ENUM_RARITY)6;
                if (!ScanStr(&s))
                {
                    break;
                }
                // b28d-solo-ServerParameterScript: ORIG 0x08a8d70e 段
                // `make_pairIR11ENUM_RARITYR15ItemFarmingData` ⇒ 第一参类型为
                // ENUM_RARITY 左值（p167 MISSING 该实例 1 + pair C1I11ENUM_RARITYS1_
                // 1 + pair D1 2 / EXTRA make_pairIRiR15ItemFarmingData 1）。
                if (s == "[common]")
                {
                    rarity = (ENUM_RARITY)0;
                }
                else if (s == "[uncommon]")
                {
                    rarity = (ENUM_RARITY)1;
                }
                else if (s == "[rare]")
                {
                    rarity = (ENUM_RARITY)2;
                }
                else if (s == "[unique]")
                {
                    rarity = (ENUM_RARITY)3;
                }
                else if (s == "[epic]")
                {
                    rarity = (ENUM_RARITY)4;
                }
                else if (s == "[chronicle]")
                {
                    rarity = (ENUM_RARITY)5;
                }
                while (true)
                {
                    itemId = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    weight = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    data.m_active += weight;
                    data.m_vecWeight.push_back(std::pair<int, int>(itemId, weight));
                }
                script->m_farming4c0.insert(std::make_pair(rarity, data));
            }
        }
        else if (line == "[drop bonus of dungeon level increase]")
        {
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                float b = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                // b28d-solo-ServerParameterScript: ORIG 用 push_backERKS1_（左值实参）
                // ⇒ 具名 pair 局部再传（p167 MISSING push_backERKS1_ 1 / EXTRA
                // push_backEOS1_ 1）。
                std::pair<int, float> entry = std::make_pair(a, b);
                script->m_vec4d8.push_back(entry);
            }
        }
        // [N-类·跨版本 EH] 本函数 4 处局部对象段(auction480/chaosWeight4e4/
        // ghostLevel4f0/ghostChaos4fc，ORIG tag $0xd67/$0xd8b/$0xda3/$0xdbb)我们
        // 各多 1 个 8 行 landing pad(mov%edx,%ebx;mov%eax,%esi;lea obj;call D1;
        // 还原;jmp)。ORIG=gcc4.1 SJLJ EH：清理走函数级 personality，无 per-call
        // landing pad(ORIG @0x08a87f16 后 4 段实测 0 条 cleanup)；4.4.7 DW2 每
        // potential-throw 区生成。正常路径 D1 调用两侧一致，仅冷路径多。豁免。
        else if (line == "[auction register item limit]")
        {
            while (true)
            {
                stAuctionLimitItemParam_t p;
                p.m_itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_startSec = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_endSec = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_auction480.push_back(p);
            }
        }
        else if (line == "[my assault chaos point weight]")
        {
            while (true)
            {
                stAssaultChaosPointWeight w;
                w.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                w.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                w.m_w0 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                w.m_w1 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_chaosWeight4e4.push_back(w);
            }
        }
        else if (line == "[assault level ghost panalty]")
        {
            while (true)
            {
                stAssaultLevelGhostPanalty g;
                g.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                g.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_t0 = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_t1 = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_ghostLevel4f0.push_back(g);
            }
        }
        else if (line == "[assault chaos point ghost panalty]")
        {
            while (true)
            {
                stAssaultChaosPointGhostPanalty g;
                g.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                g.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_r0 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                g.m_r1 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_ghostChaos4fc.push_back(g);
            }
        }
        else
        {
        }
        if (missed)
        {
            printf("MISSED SCRIPT TAG [%s] %s\n", path, line.c_str());
        }
    }
lab_finish:
    // ★ ORIG 0x08a8c23f 尾形：`cmp $0xffffffff,%eax; jne L; mov $0, %ebx; jmp C;
    //   L: mov $1,%ebx; C: <~string>; mov %ebx,%eax; ret` ⇒ 返回值是**完整 32 位**
    //   0/1。旧转写 `return script->m_558 != -1;`（bool 返回）被 4.4.7 折成
    //   `setne %bl`（我方 0x086bce4d 实证），只有低 8 位被写，eax 高 24 位残留
    //   上一次 ebx ⇒ Frida A/B 实测 ret=0x0924f301（= readCharFromBuffer::charBuffer+1），
    //   ORIG 为 1。改成两段式 return 即复现 ORIG 的分支形。
    if (script->m_558 == -1)
    {
        return 0;
    }
    return 1;
}