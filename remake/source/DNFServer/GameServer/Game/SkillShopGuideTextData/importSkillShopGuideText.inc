int importSkillShopGuideText(
    const char* path,
    std::map<std::pair<ENUM_CHARACTERJOB, int>,
             std::vector<SkillShopGuideTextData, std::allocator<SkillShopGuideTextData> >,
             std::less<std::pair<ENUM_CHARACTERJOB, int> >,
             std::allocator<std::pair<std::pair<ENUM_CHARACTERJOB, int> const,
             std::vector<SkillShopGuideTextData, std::allocator<SkillShopGuideTextData> > > > >& out)
{
    typedef std::map<std::pair<ENUM_CHARACTERJOB, int>,
                     std::vector<SkillShopGuideTextData,
                                 std::allocator<SkillShopGuideTextData> >,
                     std::less<std::pair<ENUM_CHARACTERJOB, int> >,
                     std::allocator<std::pair<std::pair<ENUM_CHARACTERJOB, int> const,
                     std::vector<SkillShopGuideTextData,
                                 std::allocator<SkillShopGuideTextData> > > > > OutMap;
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3318,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
        return 0;
    }

    std::pair<ENUM_CHARACTERJOB, int> key;
    key.first = (ENUM_CHARACTERJOB)0xb;
    key.second = -1;
    SkillShopGuideTextData data;
    std::vector<SkillShopGuideTextData> vec;
    std::string token;

    // ORIG 0x08914532：循环条件在顶部（入口直落 ScanType，失败 jne 出口）；
    // 4.4 对 while 恒旋转 ⇒ 用 for(;;)+break 复现（P164）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        // ORIG 0x08914d35 的 goto 回跳目标是 0x08914553（__lookup guard 检查）⇒
        // loop 标签在三个 static 声明之前，回跳时重新过 guard 检查（P177 同族）。
    loop:
        ;
        typedef std::map<const char*, int, less_TCHAR> LookupMap;
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int type;

        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(token.c_str());
            // ORIG 0x08914704/08914709：两臂经 %eax 汇聚后统一 mov %eax,-0x1c ⇒ 三目形（P47）。
            type = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto loop;
            }
            type = 0;
        }

        switch (type)
        {
        case 0:
        case 0x332c:   // "[guide]"
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[guide]", 0x332c));
            else
                break;
            // fall through
        case 0x3330:   // "[/guide]"
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/guide]", 0x3330));
            else
                break;
            // fall through
        case 0x3334:   // "[character job]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[character job]", 0x3334));
            }
            else
            {
                std::string jobStr;
                // ORIG 0x0891490c/0891495a 的 ok 值走 %esi（无栈槽）⇒ 纯 register
                // 存储类提示（P231；非 asm 绑定，P0 合规）。
                // ORIG 0x089149b6 用 32 位 test %esi,%esi ⇒ ok 取 int 宽度（纯 register
                // 存储类提示，P231；非 asm 绑定，P0 合规）。
                register int ok = ScanStr(&jobStr);
                if (ok)
                {
                    key.first = (ENUM_CHARACTERJOB)getCharacterJob(jobStr.c_str());
                    std::string growStr;
                    ok = ScanStr(&growStr);
                    if (ok)
                        key.second = getGrowTypeIndex(growStr.c_str());
                }
                if (!ok)
                    return 0;
                break;
            }
            // fall through
        case 0x3342:   // "[/character job]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/character job]", 0x3342));
            }
            else
            {
                // ORIG 0x08914a7b：直接就地构造 out 的 value_type
                // （_ZNSt4pairIKS_I17ENUM_CHARACTERJOBiESt6vectorI22SkillShopGuideTextData
                //   SaIS4_EEEC1IRS1_RS6_EEOT_OT0_ = pair<const pair<JOB,int>,vector>
                //   的 pair<pair<JOB,int>&, vector&> 模板构造），随后 map::insert；
                // make_pair 会多出 pair<pair<JOB,int>,vector> → value_type 的转换临时。
                out.insert(OutMap::value_type(key, vec));
                key.first = (ENUM_CHARACTERJOB)0xb;
                key.second = -1;
                vec.clear();
                break;
            }
            // fall through
        case 0x334c:   // "[text info]"
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[text info]", 0x334c));
            else
                break;
            // fall through
        case 0x3350:   // "[/text info]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[/text info]", 0x3350));
            }
            else
            {
                // ORIG 0x08914b92：__inited==1 路径把当前 data 推入 vec
                // （_ZNSt6vectorI22SkillShopGuideTextDataSaIS0_EE9push_backERKS0_，
                //   p167 差集曾 MISSING×1）。
                vec.push_back(data);
                break;
            }
            // fall through
        case 0x3355:   // "[text]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[text]", 0x3355));
            }
            else
            {
                ScanStr(&data.m_text);
                break;
            }
            // fall through
        case 0x335a:   // "[color]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[color]", 0x335a));
            }
            else
            {
                data.m_colorR = (unsigned char)ScanInt((bool*)0);
                data.m_colorG = (unsigned char)ScanInt((bool*)0);
                data.m_colorB = (unsigned char)ScanInt((bool*)0);
                break;
            }
            // fall through
        case 0x3361:   // "[level range]"
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[level range]", 0x3361));
            }
            else
            {
                data.m_levelMin = ScanInt((bool*)0);
                data.m_levelMax = ScanInt((bool*)0);
                break;
            }
            break;
        default:
            break;
        }

        // ORIG 0x08914d14：`movzbl __inited; xor $0x1,%eax; test %al,%al; je <跳过>`
        // ⇒ 原码是负极性正向包裹（P168a/P194）；写 `if(__inited) continue;` 4.4 产
        // `test;jne` 反极性且少一条 xor。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto loop;
        }
    }

    return 1;
