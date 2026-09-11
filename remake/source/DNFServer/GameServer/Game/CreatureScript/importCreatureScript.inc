bool importCreatureScript(CreatureScript* script, const char* path)
{
    // [b28d-solo-CreatureScript] 超大件（ORIG 2168 条）§6.2 三件套封顶：
    // ① p167 调用点差集 **CLEAR**（ORIG calls=330 / ours=330，0 missing / 0 extra）；
    // ② gap = 25（2168 vs 2143）；③ 帧尺寸 ORIG 0x2ac vs ours 0x2dc（N82/N115）；
    // ④ 逐块比对：store 偏移多重集与 ORIG **完全一致**（无字段错位）；残差块 64 个，
    //    分类为 X 族取反物化（P168a④ N 封顶，13 块）、4.1 循环前 continue 岛
    //    （0x08a35ed9..0x08a35ee2 四岛，P223 多岛）、帧/槽平移（N82）、
    //    dead 局部（P232，ORIG -0x22c int/-0x22d char）、EH cleanup 的 esi 值。
    //    已修：NODE_END case 改否定式 `if (!__inited)`（0x08a37a13 xor 形）+
    //    m_directionCorrect 直接布尔赋值（0x8a360xx setne 形）⇒ 块数 68→64 零回归。
    // [b290-TakeExpert P168a① 攻法] 注册链 11 段（CSTAG_SKILL_TYPE..CSTAG_ATTACK_INFO）
    //    由 `if (__inited) { parse; break; } insert;` 改写为 `if (!__inited) { insert; }
    //    else { parse; break; }`（语义等价：true→parse+break；false→insert 后自然
    //    fall-through 下一 case）。4.4 对内存 bool 取反即产 ORIG 同形
    //    `movzbl __inited; xor $1; test %al,%al; je <parse>`（P168a① 裸形；NODE_END
    //    前已用此形）。同一 .o 实测：origN 2188 不变，ourN 2157→2178 ⇒ gap 31→10；
    //    structdiff 672→185；ndiff 612→516。p167 修正版 calls=330/330，0 游戏域
    //    callee 差（仅 C1/C2 镜像，附二 §2 豁免）。
    // 语义完成（p167 归零 + 偏移集一致），byte 层 N82/N89/P182 封顶。
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();
    getDirectoryPath(script->m_directory, "%s%s", g_creatureScriptBaseDirectory, path);

    std::string tag;
    std::string token;
    while (ScanType(tag, true))
    {
        if (tag == "[name]")
        {
            ScanStr(&script->m_name);
        }
        else if (tag == "[character span]")
        {
            script->m_field18 = ScanInt((bool*)0);
        }
        else if (tag == "[width]")
        {
            script->m_field8 = ScanInt((bool*)0);
            script->m_fieldc = ScanInt((bool*)0);
        }
        else if (tag == "[floating height]")
        {
            script->m_field10 = ScanInt((bool*)0);
        }
        else if (tag == "[layer]")
        {
            ScanStr(&token);
            script->m_layer = getDrawLayer(token.c_str());
        }
        else if (tag == "[skill recovery time]")
        {
            script->m_field1c = ScanInt((bool*)0);
        }
        else if (tag == "[over skill recovery time]")
        {
            script->m_field20 = ScanInt((bool*)0);
        }
        else if (tag == "[direction correct]")
        {
            // [b28d-solo] ORIG 0x08a360xx 段：`call ScanInt; test %eax,%eax;
            // setne %dl; mov this,%eax; mov %dl,0x24(%eax)` ⇒ 源形为直接布尔
            // 赋值（三目 `?1:0` 4.4 恒产 je/mov$1/jmp/mov$0 分支块）。
            script->m_directionCorrect = (ScanInt((bool*)0) != 0);
        }
        else if (tag == "[move speed]")
        {
            // ORIG 0x8a360d2..0x8a3611a：五段独立调用点（非线性循环）。
            script->m_moveSpeedLo[0] = ScanInt((bool*)0);
            script->m_moveSpeedLo[1] = ScanInt((bool*)0);
            script->m_moveSpeedHi[0] = ScanInt((bool*)0);
            script->m_moveSpeedHi[1] = ScanInt((bool*)0);
            script->m_moveSpeedHi[2] = ScanInt((bool*)0);
        }
        else if (tag == "[gravity]")
        {
            script->m_gravity = ScanInt((bool*)0);
        }
        else if (tag == "[start level]")
        {
            script->m_startLevel = ScanInt((bool*)0);
        }
        else if (tag == "[permission level]")
        {
            script->m_permissionLevel = ScanInt((bool*)0);
        }
        else if (tag == "[max level]")
        {
            script->m_maxLevel = ScanInt((bool*)0);
        }
        else if (tag == "[parent max level]")
        {
            script->m_parentMaxLevel = ScanInt((bool*)0);
        }
        else if (tag == "[artifact slot]")
        {
            while (ScanStr(&token))
            {
                if (token == "red")
                    script->m_artifactSlotRed = 1;
                else if (token == "blue")
                    script->m_artifactSlotBlue = 1;
                else if (token == "green")
                    script->m_artifactSlotGreen = 1;
            }
        }
        else if (tag == "[learn overskill level]")
        {
            script->m_learnOverSkillLevel = ScanInt((bool*)0);
        }
        else if (tag == "[skill MP]")
        {
            script->m_skillMP = ScanInt((bool*)0);
        }
        else if (tag == "[over skill MP]")
        {
            script->m_overSkillMP = ScanInt((bool*)0);
        }
        else if (tag == "[skill type]")
        {
            ScanStr(&token);
            if (token == "[active]")
                script->m_skillType = 0;
            else if (token == "[passive]")
                script->m_skillType = 1;
        }
        else if (tag == "[over skill type]")
        {
            ScanStr(&token);
            if (token == "[active]")
                script->m_overSkillType = 0;
            else if (token == "[passive]")
                script->m_overSkillType = 1;
        }
        else if (tag == "[basic motion]")
        {
            ScanStr(&script->m_basicMotion);
        }
        else if (tag == "[walk motion]")
        {
            ScanStr(&script->m_walkMotion);
        }
        else if (tag == "[run motion]")
        {
            ScanStr(&script->m_runMotion);
        }
        else if (tag == "[fly motion]")
        {
            ScanStr(&script->m_flyMotion);
        }
        else if (tag == "[get exp motion]")
        {
            ScanStr(&script->m_getExpMotion);
        }
        else if (tag == "[get exp over motion]")
        {
            ScanStr(&script->m_getExpOverMotion);
        }
        else if (tag == "[get exp under motion]")
        {
            ScanStr(&script->m_getExpUnderMotion);
        }
        else if (tag == "[skill motion]")
        {
            ScanStr(&script->m_skillMotion);
        }
        else if (tag == "[over skill motion]")
        {
            ScanStr(&script->m_overSkillMotion);
        }
        else if (tag == "[response motion]")
        {
            ScanStr(&script->m_responseMotion);
        }
        else if (tag == "[special motion]")
        {
            ScanStr(&script->m_specialMotion);
        }
        else if (tag == "[equip motion]")
        {
            ScanStr(&script->m_equipMotion);
        }
        else if (tag == "[equip over motion]")
        {
            ScanStr(&script->m_equipOverMotion);
        }
        else if (tag == "[equip under motion]")
        {
            ScanStr(&script->m_equipUnderMotion);
        }
        else if (tag == "[basic under effect]")
        {
            ScanStr(&script->m_basicUnderEffect);
        }
        else if (tag == "[walk under effect]")
        {
            ScanStr(&script->m_walkUnderEffect);
        }
        else if (tag == "[run under effect]")
        {
            ScanStr(&script->m_runUnderEffect);
        }
        else if (tag == "[skill under effect]")
        {
            ScanStr(&script->m_skillUnderEffect);
        }
        else if (tag == "[over skill under effect]")
        {
            ScanStr(&script->m_overSkillUnderEffect);
        }
        else if (tag == "[response under effect]")
        {
            ScanStr(&script->m_responseUnderEffect);
        }
        else if (tag == "[special under effect]")
        {
            ScanStr(&script->m_specialUnderEffect);
        }
        else if (tag == "[basic over effect]")
        {
            ScanStr(&script->m_basicOverEffect);
        }
        else if (tag == "[walk over effect]")
        {
            ScanStr(&script->m_walkOverEffect);
        }
        else if (tag == "[run over effect]")
        {
            ScanStr(&script->m_runOverEffect);
        }
        else if (tag == "[skill over effect]")
        {
            ScanStr(&script->m_skillOverEffect);
        }
        else if (tag == "[over skill over effect]")
        {
            ScanStr(&script->m_overSkillOverEffect);
        }
        else if (tag == "[response over effect]")
        {
            ScanStr(&script->m_responseOverEffect);
        }
        else if (tag == "[special over effect]")
        {
            ScanStr(&script->m_specialOverEffect);
        }
        else if (tag == "[additional animation]")
        {
            // ORIG 0x08a36991：-0x40 常量 -1（本 case 内无任何写回，cmp 恒
            // 不等分支不可达——ORIG 死代码，照录）；两 string 均复用 token
            // 槽（-0x228）。命中时临时对象 materialize（-0x220）→
            // vector<CAA>::push_back(T&&)（EO 0x8a40a76）→ dtor。
            int animField0 = -1;
            ScanStr(&token);
            int animField4 = ScanInt((bool*)0);
            ScanStr(&token);
            if (animField0 != -1)
            {
                script->m_additionalAnimations.push_back(
                    CreatureAdditionalAnimation(animField4, animField0, token.c_str()));
            }
        }
        else if (tag == "[default particle]")
        {
            ScanStr(&script->m_defaultParticle);
        }
        else if (tag == "[response sound tag]")
        {
            ScanStr(&script->m_responseSoundTag);
        }
        else if (tag == "[wing sound tag]")
        {
            ScanStr(&script->m_wingSoundTag);
        }
        else if (tag == "[walk sound tag]")
        {
            ScanStr(&script->m_walkSoundTag);
        }
        else if (tag == "[run sound tag]")
        {
            ScanStr(&script->m_runSoundTag);
        }
        else if (tag == "[revision x]")
        {
            script->m_revisionX = ScanInt((bool*)0);
        }
        else if (tag == "[revision y]")
        {
            script->m_revisionY = ScanInt((bool*)0);
        }
        else if (tag == "[default random motion rate]")
        {
            script->m_defaultRandomMotionRate = ScanInt((bool*)0);
        }
        else if (tag == "[increase random motion rate]")
        {
            script->m_increaseRandomMotionRate = ScanInt((bool*)0);
        }
        else if (tag == "[etc motion]")
        {
            script->m_etcMotion.clear();
            while (ScanStr(&token))
                script->m_etcMotion.push_back(token);
        }
        else if (tag == "[random motion]")
        {
            script->m_randomMotion.clear();
            while (ScanStr(&token))
                script->m_randomMotion.push_back(token);
        }
        else if (tag == "[attack info]")
        {
            script->m_attackInfo.clear();
            while (ScanStr(&token))
                script->m_attackInfo.push_back(token);
        }
        else if (tag == "[skill string]")
        {
            script->m_skillString.clear();
            while (ScanStr(&token))
                script->m_skillString.push_back(token);
        }
        else if (tag == "[skill level values]")
        {
            script->m_skillLevelValues.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_skillLevelValues.push_back(v);
            }
        }
        else if (tag == "[over skill level values]")
        {
            script->m_overSkillLevelValues.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_overSkillLevelValues.push_back(v);
            }
        }
        else if (tag == "[piercing]")
        {
            script->m_piercing.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_piercing.push_back(v);
            }
        }
        else if (tag == "[int data]")
        {
            script->m_intData.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_intData.push_back(v);
            }
        }
        else if (tag == "[string data]")
        {
            script->m_stringData.clear();
            while (ScanStr(&token))
                script->m_stringData.push_back(token);
        }
        else if (tag == "[evolution creature multy id]")
        {
            script->m_evolutionMultyId.clear();
            int count = ScanInt((bool*)0);
            for (int i = 0; i < count; ++i)
            {
                bool ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG 0x8a37019：先 materialize prvalue 临时（-0x214）再
                // push_back(int&&)（EO 0x0808e1ee）——写 int(v) 复现。
                script->m_evolutionMultyId.push_back(int(v));
            }
        }
        else if (tag == "[evolution quest]")
        {
            script->m_evolutionQuestId = ScanInt((bool*)0);
        }
        else if (tag == "[evolution creature id]")
        {
            script->m_evolutionCreatureId = ScanInt((bool*)0);
        }
        else if (tag == "[evolution level]")
        {
            script->m_evolutionLevel = ScanInt((bool*)0);
        }
        else if (tag == "[selective grow creature]")
        {
            // ORIG 0x8a3710d：数据先写栈上 selectiveGrowCreatureInfo（-0x24c，
            // ctor 0x0833df50），仅命中闭合标签时整体 7 字拷回 script+0x18c；
            // 文件截断（ScanType 失败）直接回主循环、不落盘。
            selectiveGrowCreatureInfo grow;
            bool growActive = true;
            while (growActive)
            {
                if (ScanType(tag, true) != 1)
                    break;
                if (tag == "[/selective grow creature]")
                {
                    script->m_selectiveGrow = grow;
                    growActive = false;
                }
                else if (tag == "[item count for levelup]")
                    grow.m_growItemCount = ScanInt((bool*)0);
                else if (tag == "[using exp item]")
                    grow.m_usingExpItem = (ScanInt((bool*)0) != 0) ? 1 : 0;
                else if (tag == "[grow Atype item index]")
                    grow.m_growATypeItem = ScanInt((bool*)0);
                else if (tag == "[grow Btype item index]")
                    grow.m_growBTypeItem = ScanInt((bool*)0);
                else if (tag == "[grow Atype creature id]")
                    grow.m_growATypeCreature = ScanInt((bool*)0);
                else if (tag == "[grow Btype creature id]")
                    grow.m_growBTypeCreature = ScanInt((bool*)0);
                else if (tag == "[grow level]")
                    grow.m_growLevel = ScanInt((bool*)0);
            }
        }
        else if (tag == "[event evolute creature]")
        {
            script->m_eventEvlouteCreature.clear();
            bool ok = false;                       // ORIG 循环头单调用点
            int v;
            for (;;)
            {
                v = ScanInt(&ok);
                if (ok != true)
                    break;
                script->m_eventEvlouteCreature.push_back(v);
            }
        }
        else if (tag == "[item evolution]")
        {
            // ORIG 0x8a373a2：首 int 失败 → log 0x259e → return false；
            // 循环内 first(-0x25c)/second(-0x258) 相邻 = st_crature_id_item_id
            // 两字段直写栈；first 读尽 → log 0x25a9(size) 后走收尾；
            // second 缺 → log 0x25af(size) → return false；first==-1 → 走收尾。
            bool ok = false;
            int itemId = ScanInt(&ok);
            if (ok != true)
            {
                LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                      0x259e,
                                      "creature script loading  %s, evolution_item_id_",
                                      path);
                return false;
            }
            script->m_itemEvolution = itemId;
            script->m_itemEvolCreatureIds.clear();
            st_crature_id_item_id entry;
            for (;;)
            {
                entry.m_creatureId = ScanInt(&ok);
                if (ok != true)
                {
                    LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                          0x25a9,
                                          "creature script loading  %s, item_evolution_creature_id_ size : %d",
                                          path, script->m_itemEvolCreatureIds.size());
                    break;
                }
                entry.m_itemId = ScanInt(&ok);
                if (ok != true)
                {
                    LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                          0x25af,
                                          "creature script loading  %s, item_evolution_creature_id_ size : %d",
                                          path, script->m_itemEvolCreatureIds.size());
                    return false;
                }
                if (entry.m_creatureId == -1)
                    break;
                script->m_itemEvolCreatureIds.push_back(entry);
            }
            // ORIG 0x8a3754b：收尾多消费一行——EOF/闭合标签 → 函数收束；
            // 否则回主循环头（该行被重扫覆盖，ORIG 固有 quirk，照录）。
            if (ScanType(tag, true) != 1)
                break;
            if (tag == "[/item evolution]")
                break;
            continue;
        }
        else if (tag == "[skill info]")
        {
            ScanStr(&token);
            script->m_skillInfo = token;
        }
        else if (tag == "[over skill info]")
        {
            ScanStr(&token);
            script->m_overSkillInfo = token;
        }
        else if (tag == "[skill list]")
        {
            // ORIG 0x8a3776f：函数局部静态三件套（_ZZ..importCreatureScript..
            // __lookup/__init_lock 各带 __cxa_guard+__cxa_atexit；__inited 为
            // 零初始化 plain bool 无 guard）。子标签经 map<const char*,int,
            // less_TCHAR> 查找分派（非线性 string==）；!__inited 时首行进
            // 注册链：case 0 起逐 case insert 后贯穿（0x8a37a6b→0x8a37d16→
            // …→0x8a38219），公共尾 leave+__inited=1 再重查同一行。
            typedef std::map<const char*, int, less_TCHAR> LookupMap;
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            bool listActive = true;              // -0x25
            while (listActive)
            {
                if (ScanType(tag, true) != 1)
                    break;                       // 0x8a37664 → 主循环头
                int key = -1;                    // -0x260
                int typeFlag = -1;               // -0x24（0xffffffff 初值）
                CreatureScriptSkillInfo info;    // -0x28c（0x8a3dd26 每轮构造）
                if (tag == "[/skill list]")
                {
                    listActive = false;          // 0x8a376a2
                    continue;                    // 0x8a3824d → info dtor → 尾检
                }
                if (tag == "[skill node]")      // 未知行 → 0x8a3824d 跳过（ORIG
                {                               // 为两次 == 直链，非 !=）
                key = -1;                        // 0x8a376c9
                info.clear();                    // 0x8a3de50
                bool ok = false;                 // -0x22d（ORIG 全函数共用槽）
                key = ScanInt(&ok);
                if (ok != true)
                {
                    LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                          0x25ea,
                                          "creature script loading  %s, evolution_item_id_",
                                          path);
                    return false;                // 0x8a37731
                }
                bool nodeActive = true;          // -0x1d
                for (;;)
                {
                    if (ScanType(tag, true) != 1)
                        break;                   // 0x8a37769：截断 → 本 node 不 insert
                redo_lookup:
                    int tagId;
                    if (__inited)
                    {
                        LookupMap::const_iterator it = __lookup.find(tag.c_str());
                        if (it != __lookup.end())
                            tagId = it->second;
                        else
                            tagId = -1;          // 0x8a37925 未命中
                    }
                    else
                    {
                        __init_lock.enter();     // 0x8a3792f
                        if (__inited)            // 竞争双检：释放后重查本行
                        {
                            __init_lock.leave(); // 0x8a3794d
                            goto redo_lookup;    // 0x8a37953 → 0x8a3776f
                        }
                        tagId = 0;               // 0x8a37958 首行注册入口
                    }
                    switch (tagId)
                    {
                    case 0:                      // 与 [/skill node] 共体
                    case CSTAG_NODE_END:
                        // [b28d-solo] ORIG 0x08a37a13 形为 `movzbl __inited; xor $1;
                        // test %al; je <已注册体>` ⇒ 源码用**否定式** `if (!__inited)`
                        // （注册体在前、解析体在 else），与 P222 否定早返同族。
                        if (!__inited)
                        {
                            __lookup.insert(LookupMap::value_type("[/skill node]", static_cast<int>(CSTAG_NODE_END)));
                        }
                        else
                        {
                            nodeActive = false;  // 0x8a37a73
                            if (key == -1 || typeFlag == -1)
                            {
                                LogManager::logFormat(0, "../RDARScriptObject.cpp",
                                                      __FUNCTION__, 0x25fc,
                                                      "creature script loading  %s, evolution_item_id_",
                                                      path);
                                return false;    // 0x8a37a88
                            }
                            if (typeFlag == 0)
                            {
                                // ORIG 0x8a37ad4：find==end 才 insert（弱实例
                                // 0x8a40b38/b0c/b5e/b72/d2c/d66 序列）。
                                if (script->m_skillList.find(key) == script->m_skillList.end())
                                    script->m_skillList.insert(
                                        std::make_pair<int, CreatureScriptSkillInfo>(key, info));
                            }
                            else
                            {
                                if (script->m_overSkillList.find(key) == script->m_overSkillList.end())
                                    script->m_overSkillList.insert(
                                        std::make_pair<int, CreatureScriptSkillInfo>(key, info));
                            }
                            break;
                        }
                        /* fall through（0x8a37a6b jmp 0x8a37d16） */
                    case CSTAG_SKILL_TYPE:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill type]", static_cast<int>(CSTAG_SKILL_TYPE)));
                        }
                        else
                        {
                            ScanStr(&token);     // 0x8a37d73（无括号 "over skill"）
                            typeFlag = (token == "over skill") ? 1 : 0;
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_NAME:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill name]", static_cast<int>(CSTAG_SKILL_NAME)));
                        }
                        else
                        {
                            ScanStr(&info.m_str0);
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_EXPLAIN:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill explain]", static_cast<int>(CSTAG_SKILL_EXPLAIN)));
                        }
                        else
                        {
                            ScanStr(&info.m_str4);
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_RECOVERY:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill recovery time]", static_cast<int>(CSTAG_SKILL_RECOVERY)));
                        }
                        else
                        {
                            info.m_field8 = ScanInt(&ok);   // 0x8a37eef（&-0x22d）
                            break;
                        }
                        /* fall through */
                    case CSTAG_LEARN_SKILL_LEVEL:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[learn skill level]", static_cast<int>(CSTAG_LEARN_SKILL_LEVEL)));
                        }
                        else
                        {
                            info.m_fieldc = ScanInt(&ok);   // 0x8a37f6e
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_MP:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill mp]", static_cast<int>(CSTAG_SKILL_MP)));
                        }
                        else
                        {
                            info.m_field10 = ScanInt(&ok);  // 0x8a37fed（+0x10 int）
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_MOTION:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill motion]", static_cast<int>(CSTAG_SKILL_MOTION)));
                        }
                        else
                        {
                            ScanStr(&info.m_str14);
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_UNDER_EFFECT:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill under effect]", static_cast<int>(CSTAG_SKILL_UNDER_EFFECT)));
                        }
                        else
                        {
                            ScanStr(&info.m_str18);
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_OVER_EFFECT:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill over effect]", static_cast<int>(CSTAG_SKILL_OVER_EFFECT)));
                        }
                        else
                        {
                            ScanStr(&info.m_str1c);
                            break;
                        }
                        /* fall through */
                    case CSTAG_SKILL_INFO:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[skill info]", static_cast<int>(CSTAG_SKILL_INFO)));
                        }
                        else
                        {
                            ScanStr(&info.m_str20);
                            break;
                        }
                        /* fall through */
                    case CSTAG_ATTACK_INFO:
                        if (!__inited)
                        {
                        __lookup.insert(LookupMap::value_type("[attack info]", static_cast<int>(CSTAG_ATTACK_INFO)));
                        }
                        else
                        {
                            ScanStr(&info.m_str24);         // 0x8a38202（漏项补全）
                            break;
                        }
                        break;                               // 注册链终点 → 公共尾
                    default:
                        // -1 未命中/未处理 → ORIG 无操作直落公共尾。
                        break;
                    }
                    // 公共尾 0x8a38219。
                    if (!__inited)
                    {
                        __init_lock.leave();     // 0x8a38227
                        __inited = true;         // 0x8a38233
                        goto redo_lookup;        // 0x8a3823a 重查同一行
                    }
                    if (!nodeActive)
                        break;                   // 0x8a38240 → info dtor → 外层
                }
                }                                  // [skill node] 块结束
            }
        }
        else if (tag == "[using random skill]")
        {
            bool ok = false;
            int v = ScanInt(&ok);
            if (ok != true)
            {
                LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                                      0x266c,
                                      "creature script loading  %s, evolution_item_id_",
                                      path);
                return false;
            }
            script->m_usingRandomSkill = (v != 0) ? 1 : 0;
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2674,
                                  "creature script loading error : %s : %s",
                                  tag.data(), path);   // 0x8a3832f = string::data
            return false;
        }
    }

    // 末尾 0x8a38376：STScriptFileList::FindIndexByFullScan(this=&列表, path)
    // 写回 m_id 后 return true（g_creatureScriptFileList 建模为 map，布局与
    // STScriptFileList +0x00 一致 → 类型视图调用，同 InitScriptProducers_5 先例）。
    script->m_id = g_creatureScriptFileList.FindIndexByFullScan(path);
    return true;
}