bool ImportAttackInfoScript(AttackInfo* info, const char* path, int param3)
{
    // [b28d-solo-CreatureScript] 超大件（ORIG 2141 条）§6.2 三件套封顶：
    // ① p167 调用点差集 **CLEAR**（ORIG calls=254 / ours=254，0 missing / 0 extra）；
    // ② gap = 84（2141 vs 2057）；③ 帧尺寸 ORIG 0x40c vs ours 0x41c（N82/N115）；
    // ④ 逐块比对：store 偏移多重集与 ORIG **完全一致**（`fstps 0x3c/0x11c/0x120/
    //    0x124` 等错位均为 difflib 对齐假象，无字段错位）；残差块 133 个，主类 =
    //    X 族取反物化（P168a④，42 块，`movzbl __inited; xor $1; test; je` 注册链）、
    //    帧/槽平移（N82）、4.1 注册链 fall-through 岛（P223）、EH cleanup esi 值。
    // [b290-TakeExpert P168a① 攻法] 注册链 39 段改写为否定式包裹
    //    `if (!__inited) { insert; } else { parse; break; }`（语义等价，同
    //    importCreatureScript 判词）⇒ 同一 .o 实测：origN 2148 不变，ourN 2066→2146
    //    ⇒ gap 82→2；structdiff 436→21；ndiff 1032→540。p167 修正版 calls=254/254，
    //    0 游戏域 callee 差（仅 D1/D2 镜像，附二 §2 豁免）。
    // 语义完成（p167 归零 + 偏移集一致），byte 层 N82/N89/P182 封顶。
    if (!loadRDARScriptFile("", path))        // 0x1a:失败 ebx=0 → return false
    {
        return false;
    }
    info->clear();

    std::string tag;                          // -0x3b4 主行标签(ScanType 读入)
    std::string token;                        // -0x3b8 子 token(ScanTypeOrStr 读入)
    int statusIdx = 0;                        // -0x34 文件级 active status 槽计数
    // module 名缓存(元素集 / active status 各一):初值 MODULE_NAME_TAG+0x18 =
    // g_moduleNameTag[3][0],与 clear() 的 m_moduleTag=3 对齐 → 首遇不误重置。
    std::string elemModule(g_moduleNameTag[3][0]);     // -0x3bc
    std::string statusModule(g_moduleNameTag[3][0]);   // -0x3c0

    for (;;)                                  // 行循环:读行 → module 过滤 → 处理
    {
        if (!ScanType(tag, true))             // 0x14e4;失败 → 0x3947 unload + ret 1
            break;
        int moduleA = -1;                     // 0x1505/-0x150f 两输出均初 -1
        int moduleB = -1;
        bool isModuleLine = processModuleTagSet(moduleA, moduleB, &tag, param3, false);
        if (moduleB != -1)                    // 0x10d:仅第二输出写 +0x8c
            info->m_moduleTag = moduleB;
        if (isModuleLine)                     // 0x1572:false=已消费 → 读下一行
        {
            // ORIG 函数局部静态三件套(各带 __cxa_guard;位于 0x11d 重查点)。
            // map<const char*,int> 键比较器用 less_TCHAR(大小写无关 strcmp 语义,
            // 工程内既有类型);find(tag.c_str()) 传动态指针,默认 less 会恒 miss。
            typedef std::map<const char*, int, less_TCHAR> LookupMap;
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            for (;;)                          // 单行处理(0x1577 区;注册后同 tag 重入)
            {
                int tagId;
                if (__inited)
                {
                    LookupMap::const_iterator it = __lookup.find(tag.c_str());
                    if (it != __lookup.end())
                        tagId = it->second;
                    else
                        tagId = -1;
                }
                else
                {
                    __init_lock.enter();      // 0x2d9
                    if (__inited)             // 竞争双检:已就绪 → 释放并重查本行
                    {
                        __init_lock.leave();
                        continue;             // 0x2fc → 0x24e3 → 0x1577
                    }
                    tagId = 0;                // 0x301 首行 → 注册链入口
                }

                switch (tagId)
                {
                case 0:                       // 注册链入口(与 [damage] 共体)
                case AITAG_DAMAGE:            // +0x00 float
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[damage]", static_cast<int>(AITAG_DAMAGE)));
                    }
                    else
                    {
                        info->m_damage = ScanFloat((bool*)0);
                        break;
                    }
                    /* fall through（ORIG 0x5c8 jmp 0x5e3） */
                case AITAG_DAMAGE_INCREASE_RATE:  // +0x04 float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[damage increase rate]",
                                                          static_cast<int>(AITAG_DAMAGE_INCREASE_RATE)));
                    }
                    else
                    {
                        info->m_damageIncreaseRate = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_ABSOLUTE_DAMAGE:   // +0x08 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[absolute damage]",
                                                          static_cast<int>(AITAG_ABSOLUTE_DAMAGE)));
                    }
                    else
                    {
                        info->m_absoluteDamage = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_DAMAGE_BONUS:      // +0x0c int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[damage bonus]",
                                                          static_cast<int>(AITAG_DAMAGE_BONUS)));
                    }
                    else
                    {
                        info->m_damageBonus = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_ATTACK_TYPE:       // +0x10 枚举
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[attack type]",
                                                          static_cast<int>(AITAG_ATTACK_TYPE)));
                    }
                    else
                    {
                        bool isType;          // -0x3cd
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[physic]")
                            info->m_attackType = (AttackInfo::ENUM_ATTACKTYPE)0;
                        else if (token == "[magic]")
                            info->m_attackType = (AttackInfo::ENUM_ATTACKTYPE)1;
                        else if (token == "[ignore defense]")
                            info->m_attackType = (AttackInfo::ENUM_ATTACKTYPE)2;
                        break;
                    }
                case AITAG_WEAPON_DAMAGE_APPLY:  // +0x14 bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[weapon damage apply]",
                                                          static_cast<int>(AITAG_WEAPON_DAMAGE_APPLY)));
                    }
                    else
                    {
                        info->m_weaponDamageApply = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_ATTACK_MYSELF:     // +0x98 bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[attack myself]",
                                                          static_cast<int>(AITAG_ATTACK_MYSELF)));
                    }
                    else
                    {
                        info->m_attackFlags[0] = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_ATTACK_FRIEND:     // +0x9c bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[attack friend]",
                                                          static_cast<int>(AITAG_ATTACK_FRIEND)));
                    }
                    else
                    {
                        info->m_attackFlags[1] = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_ATTACK_ENEMY:      // +0xa0 bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[attack enemy]",
                                                          static_cast<int>(AITAG_ATTACK_ENEMY)));
                    }
                    else
                    {
                        info->m_attackFlags[2] = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_ELEMENTAL_PROPERTY:  // +0xa4 set<ENUM_ELEMENT>
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[elemental property]",
                                                          static_cast<int>(AITAG_ELEMENTAL_PROPERTY)));
                    }
                    else
                    {
                        // module 变化时清空元素集(0x0aa0..0x0ac9)
                        const std::string& curMod = g_moduleNameTag[info->m_moduleTag][0];
                        if (elemModule != curMod)
                            info->m_elements.clear();
                        elemModule = curMod;  // 0x0aec 无条件更新缓存
                        bool isType;          // -0x3ce
                        ScanTypeOrStr(isType, token, true);
                        info->setElement((ENUM_ELEMENT)getElement(token.c_str()));
                        break;
                    }
                case AITAG_ACTIVE_STATUS:     // 0xbc..0xf4 + vectors
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[active status]",
                                                          static_cast<int>(AITAG_ACTIVE_STATUS)));
                    }
                    else
                    {
                        const std::string& curMod = g_moduleNameTag[info->m_moduleTag][0];
                        if (statusModule != curMod)   // 0x0bb9 module 变化:重置槽
                        {
                            for (int i = 0; i < 3; ++i)
                            {
                                info->m_activeStatusInt3[i] = 0;          // 0xec+4i
                                info->m_activeStatusVectors[i].clear();   // 0xf8+12i
                            }
                        }
                        statusModule = curMod;        // 0x0c28 无条件更新缓存
                        if (statusIdx > 2)            // 0x0c2d 槽满
                        {
                            LogManager::logFormat(0, "../RDARScriptAttackInfo.cpp",
                                                  __FUNCTION__, 0x9c,
                                                  ".atk \xbf\xa1 [active status] \xb0\xb9\xbc\xf6 \xc3\xca\xb0\xfa");
                            continue;                 // 0x0c5f → 0x24e3 重查本行
                        }
                        bool isType;                  // -0x3cf
                        ScanTypeOrStr(isType, token, true);     // 0x0c7f
                        // 逐访问重读 statusIdx(P38:ORIG 每写前 mov -0x34)
                        info->m_activeStatusType[statusIdx] =
                            getActiveStatusType(token);          // 0xbc+4i
                        info->m_activeStatusValue[statusIdx] =
                            ScanFloat((bool*)0);                 // 0xc8+4i
                        info->m_activeStatusInt[statusIdx] =
                            ScanInt((bool*)0);                   // 0xd4+4i
                        info->m_activeStatusInt3[statusIdx] =
                            ScanInt((bool*)0);                   // 0xec+4i
                        // 按状态名追加向量初值(0x0d04..0x1308 比较直链)
                        if (token == "[blind]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[slow]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[haste]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[stone]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[confuse]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[armor break]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[weapon break]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[poison]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[lightning]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[bleeding]")
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[burn]")       // 0x111f..0x1229:4 次显式
                        {
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                        }
                        else if (token == "[bless]")      // 0x1244:循环 4 次
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            }
                        }
                        else if (token == "[curse]")      // 0x12b5:循环 4 次
                        {
                            for (int k = 0; k < 4; ++k)
                            {
                                info->m_activeStatusVectors[statusIdx].push_back(ScanInt((bool*)0));
                            }
                        }
                        ++statusIdx;                      // 0x130a
                        break;
                    }
                case AITAG_DAMAGE_REACTION:   // +0x1c 枚举
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[damage reaction]",
                                                          static_cast<int>(AITAG_DAMAGE_REACTION)));
                    }
                    else
                    {
                        bool isType;          // -0x3d0
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[none]")
                            info->m_damageReaction = 0;
                        else if (token == "[damage]")
                            info->m_damageReaction = 1;
                        else if (token == "[down]")
                            info->m_damageReaction = 2;
                        else if (token == "[damage except human]")
                            info->m_damageReaction = 3;
                        break;
                    }
                case AITAG_PUSH_ASIDE:        // +0x20 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[push aside]",
                                                          static_cast<int>(AITAG_PUSH_ASIDE)));
                    }
                    else
                    {
                        info->m_pushAside = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_LIFT_UP:           // +0x24 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[lift up]", static_cast<int>(AITAG_LIFT_UP)));
                    }
                    else
                    {
                        info->m_liftUp = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_CRITICAL_HIT:      // +0x28 float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[critical hit]",
                                                          static_cast<int>(AITAG_CRITICAL_HIT)));
                    }
                    else
                    {
                        info->m_criticalHit = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_STUCK:             // +0x2c float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[stuck]", static_cast<int>(AITAG_STUCK)));
                    }
                    else
                    {
                        info->m_stuck = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_STUCK_PROB_INCREASE:  // +0x30 bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[stuck prob increase]",
                                                          static_cast<int>(AITAG_STUCK_PROB_INCREASE)));
                    }
                    else
                    {
                        info->m_stuckProbIncrease = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_KNUCK_BACK:        // +0x34 int;==3 时 +0x38 角度
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[knuck back]",
                                                          static_cast<int>(AITAG_KNUCK_BACK)));
                    }
                    else
                    {
                        info->m_knuckBack = ScanInt((bool*)0);
                        if (info->m_knuckBack == 3)   // 0x2b56;fistpl 舍入转 int
                        {
                            bool ok = false;          // -0x3d1
                            info->m_knuckBackAngle = (int)ScanFloat(&ok);
                        }
                        break;
                    }
                case AITAG_WEAKNESS:          // +0x3c/+0x40 float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[weakness]", static_cast<int>(AITAG_WEAKNESS)));
                    }
                    else
                    {
                        info->m_weakness = ScanFloat((bool*)0);
                        info->m_weakness2 = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_BOUNCE:            // +0x44 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[bounce]", static_cast<int>(AITAG_BOUNCE)));
                    }
                    else
                    {
                        info->m_bounce = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_IGNORE_DEFENSE:    // +0x48 bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[ignore defense]",
                                                          static_cast<int>(AITAG_IGNORE_DEFENSE)));
                    }
                    else
                    {
                        info->m_ignoreDefense = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_HUMAN_DAMAGE_RATE: // +0x11c float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[human damage rate]",
                                                          static_cast<int>(AITAG_HUMAN_DAMAGE_RATE)));
                    }
                    else
                    {
                        info->m_damageRate[0] = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_FORT_DAMAGE_RATE:  // +0x120 float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[fort damage rate]",
                                                          static_cast<int>(AITAG_FORT_DAMAGE_RATE)));
                    }
                    else
                    {
                        info->m_damageRate[1] = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_MONSTER_DAMAGE_RATE:  // +0x124 float
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[monster damage rate]",
                                                          static_cast<int>(AITAG_MONSTER_DAMAGE_RATE)));
                    }
                    else
                    {
                        info->m_damageRate[2] = ScanFloat((bool*)0);
                        break;
                    }
                case AITAG_HP_DRAIN:          // +0x78 bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[HP drain]", static_cast<int>(AITAG_HP_DRAIN)));
                    }
                    else
                    {
                        info->m_hpDrain = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_MP_DRAIN:          // +0x7c bool(int)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[MP drain]", static_cast<int>(AITAG_MP_DRAIN)));
                    }
                    else
                    {
                        info->m_mpDrain = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_HIT_WAV:           // +0x4c string
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[hit wav]", static_cast<int>(AITAG_HIT_WAV)));
                    }
                    else
                    {
                        ScanStr(&info->m_hitWav);
                        break;
                    }
                case AITAG_ATTACK_DIRECTION:  // +0x50 枚举(lift up/horizon/down)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[attack direction]",
                                                          static_cast<int>(AITAG_ATTACK_DIRECTION)));
                    }
                    else
                    {
                        bool isType;          // -0x3d2
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[hit lift up]")
                            info->m_attackDirection = 0;
                        else if (token == "[hit horizon]")
                            info->m_attackDirection = 1;
                        else if (token == "[hit down]")
                            info->m_attackDirection = 2;
                        break;
                    }
                case AITAG_HIT_INFO:          // +0x54/+0x58/+0x5c/+0x60
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[hit info]", static_cast<int>(AITAG_HIT_INFO)));
                    }
                    else
                    {
                        bool isType;          // -0x3d3(两段复用同槽)
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[blow]")
                            info->m_hitInfo = 0;
                        else if (token == "[cut]")
                            info->m_hitInfo = 1;
                        else if (token == "[burn]")
                            info->m_hitInfo = 2;
                        else if (token == "[mana burn]")
                            info->m_hitInfo = 4;
                        else if (token == "[etc]")
                            info->m_hitInfo = 3;
                        ScanTypeOrStr(isType, token, true);   // 0x1dd4 [no blood]/[blood]
                        if (token == "[no blood]")
                            info->m_blood = 0;
                        else if (token == "[blood]")
                            info->m_blood = 1;
                        info->m_field5c = ScanInt((bool*)0);   // +0x5c
                        info->m_field60 = ScanFloat((bool*)0); // +0x60
                        break;
                    }
                case AITAG_IS_USE_WEAPON_HIT_DELAY_RATE:  // +0x64 byte bool
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type(
                        "[is use weapon hit delay rate]",
                        static_cast<int>(AITAG_IS_USE_WEAPON_HIT_DELAY_RATE)));
                    }
                    else
                    {
                        info->m_isUseWeaponHitDelayRate = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_FORCE_HIT_STUN_TIME:  // +0x74 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[force hit stun time]",
                                                          static_cast<int>(AITAG_FORCE_HIT_STUN_TIME)));
                    }
                    else
                    {
                        info->m_forceHitStunTime = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_HIT_DIRECTION:     // +0x68 枚举(auto/front/back/...)
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[hit direction]",
                                                          static_cast<int>(AITAG_HIT_DIRECTION)));
                    }
                    else
                    {
                        bool isType;          // -0x3d4
                        ScanTypeOrStr(isType, token, true);
                        if (token == "[auto]")
                            info->m_hitDirection = 0;
                        else if (token == "[front]")
                            info->m_hitDirection = 1;
                        else if (token == "[back]")
                            info->m_hitDirection = 2;
                        else if (token == "[outer]")
                            info->m_hitDirection = 3;
                        else if (token == "[inner]")
                            info->m_hitDirection = 4;
                        break;
                    }
                case AITAG_IGNORE_SUPER_ARMOR:  // +0x6c int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[ignore super armor]",
                                                          static_cast<int>(AITAG_IGNORE_SUPER_ARMOR)));
                    }
                    else
                    {
                        info->m_ignoreSuperArmor = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_IGNORE_WEIGHT:     // +0x70 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[ignore weight]",
                                                          static_cast<int>(AITAG_IGNORE_WEIGHT)));
                    }
                    else
                    {
                        info->m_ignoreWeight = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_HUMAN_ACTIVE_STATUS_RATE:  // +0x18 = 值/100.0f
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type(
                        "[human active status rate]", static_cast<int>(AITAG_HUMAN_ACTIVE_STATUS_RATE)));
                    }
                    else
                    {
                        float f = ScanFloat((bool*)0);   // -0x1c 中转
                        info->m_humanActiveStatusRate = f / 100.0f;
                        break;
                    }
                case AITAG_IS_HIT_NUMBER_INCREASING:  // +0x88 byte bool
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type(
                        "[is hit number increasing]", static_cast<int>(AITAG_IS_HIT_NUMBER_INCREASING)));
                    }
                    else
                    {
                        info->m_isHitNumberIncreasing = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_CATCH_OBJECT:      // +0x128 vector<CatchObjectInfo>
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[catch object]",
                                                          static_cast<int>(AITAG_CATCH_OBJECT)));
                    }
                    else
                    {
                        info->m_catchObjects.clear();    // 0x22d6
                        CatchObjectInfo obj;             // -0x3e0{int,int,float}
                        bool ok;                         // -0x3e1
                        for (;;)                         // 0x22e6 回环
                        {
                            obj.m_field0 = ScanInt(&ok); // 返回值直接入 field0
                            if (!ok)
                                break;
                            obj.m_field4 = ScanInt((bool*)0);
                            obj.m_field8 = ScanFloat((bool*)0);
                            info->m_catchObjects.push_back(obj);
                        }
                        break;
                    }
                case AITAG_MAX_TARGET_HIT_NUMBER:  // +0x94 int
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[max target hit number]",
                                                          static_cast<int>(AITAG_MAX_TARGET_HIT_NUMBER)));
                    }
                    else
                    {
                        info->m_maxTargetHitNumber = ScanInt((bool*)0);
                        break;
                    }
                case AITAG_IGNORE_DIEHARD:    // +0x90 byte bool
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[ignore diehard]",
                                                          static_cast<int>(AITAG_IGNORE_DIEHARD)));
                    }
                    else
                    {
                        info->m_ignoreDiehard = (ScanInt((bool*)0) != 0);
                        break;
                    }
                case AITAG_HIT_CHECK_BY_SCREEN_POS:  // +0x91;ScanInt 失败 → ret false
                    if (!__inited)
                    {
                    __lookup.insert(LookupMap::value_type("[hit check by screen pos]",
                                                          static_cast<int>(AITAG_HIT_CHECK_BY_SCREEN_POS)));
                    }
                    else
                    {
                        int v = 0;                       // -0x3e8
                        if (!ScanInt(&v))                // _Z7ScanIntPi 0x2485
                            return false;                // 0x2491(不经 unload)
                        info->m_hitCheckByScreenPos = (v != 0);
                        break;
                    }
                    break;                    // 注册链终点(0x2470 → 公共尾)
                default:                      // 未知标签:静默忽略(ORIG find=-1)
                    break;
                }

                if (!__inited)                // 0x24bb:注册完成 → 置位并同 tag 重处理
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;
                }
                break;                        // 行处理完成 → 读下一行
            }
        }
    }
    unloadRDARScriptFile();                   // 0x24ee
    return true;
