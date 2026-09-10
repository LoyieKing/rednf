int ImportEquipmentAniScript(std::map<std::string, STEquipmentAniScript>* table)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：1279/1225，gap=54，
    // p167 181/181 CLEAR）：本波修文件迭代器先声明后赋值（P314/P50 复活，
    // 复现 normal_iterator C1Ev）+ [equipment ani script] 终止守卫改 `==`+break
    // （P133）。
    // 附：STEquipmentAniScript::C1ERKS_（433/423）与 D1（332/365）为本 TU 使用点
    // 发射的编译器隐式 W 符号（无显式定义可挂判词）——ORIG 0x0898xxxx 段对
    // 连续 string 成员用 `mov $0x3,%ebx` 倒计数块搬移循环（前 10 个逐个
    // SsC1ERKSs、其余走循环），4.4 恒逐个展开 ⇒ N90 数组成员块搬移分块启发式
    // + N91 EH cleanup 跨版本差，判词随本函数。
    if (!loadRDARScriptFile("", "Etc/equipmentAniScript.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptItem.cpp",
                              __FUNCTION__, 0x3116,
                              "CRITICAL ERROR : EquipmentAniScript!");
        return false;
    }

    std::string section;                 // ORIG -0x358（0x089c0655 先构造）
    // 子文件名收集。ORIG vector 与 section 同区构造（0x089c0663，先于首次
    // ScanType）；循环终止比较对象是 section token（0x089c06d9 对
    // [/equipment ani script] 比较外层 section，恒假），实际以 ScanStr 失败
    // 收尾；忠实保留该比较形态。
    std::vector<std::string> files;      // ORIG -0x364
    if (!ScanType(section, true))
        return false;
    if (section == "[equipment ani script]")
    {
        std::string name;                // ORIG -0x36c（block 作用域构造/析构）
        // [b28d-solo-ScriptDepsA] ORIG 用 `section == "[/equipment ani script]"`
        // +break（p167 MISSING eq / EXTRA ne）。
        // [P292/P283] ORIG 0x089c06d2 `call; xor $0x1,%eax; test %al,%al; jne`
        // ⇒ for(;;)+`if (!ScanStr(...)) break;`。
        for (;;)
        {
            if (!ScanStr(&name))
                break;
            if (section == "[/equipment ani script]")
                break;
            files.push_back(name);
        }
    }

    // [b28d-solo-ScriptDepsA] P314/P50 复活：ORIG 0x089c0698 段先默认构造
    // iterator（C1Ev）再赋值 begin()。
    std::vector<std::string>::iterator fit;
    for (fit = files.begin(); fit != files.end(); ++fit)
    {
        if (!loadRDARScriptFile("", fit->c_str()))
            return false;

        std::string tag;                 // ORIG -0x370
        std::string val;                 // ORIG -0x374
        STEquipmentAniScript entry;      // ORIG -0x3f4（C1 0x0898ca36）

        // [P292/P283] ORIG 0x089c079c/0x089c07ed `call; xor $0x1,%eax; test;
        // jne/je` ⇒ for(;;)+`if (!ScanType(...)) break;`。
        for (;;)
        {
            if (!ScanType(tag, true))
                break;
            static AniLookupMap __lookup;    // 0x089c07f8 guard 区
            static SpinLock __init_lock;     // 0x089c0872 guard 区
            static bool __inited = false;    // 0x094fde04

            // ORIG 建表完成/锁内已建时 jmp 0x089c07f8（guard 检查区）=
            // 内层 for(;;) 重派发循环头。
            for (;;)
            {
                int id;
                if (__inited)
                {
                    const char* key = tag.c_str();   // 0x089c08fb
                    AniLookupMap::const_iterator it = __lookup.find(key);
                    id = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();             // 0x089c09bb
                    if (__inited)
                    {
                        __init_lock.leave();         // 0x089c09d2
                        continue;                    // jmp 0x089c07f8
                    }
                    id = 0;                          // 0x089c09dd
                }

                switch (id)
                {
                case 0:
                case 0x3142:                       // [LAYER]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[LAYER]", 0x3142));
                    }
                    else
                    {
                        entry.m_field0 = ScanInt(static_cast<bool*>(0));
                        break;                     // 0x089c0bd9
                    }
                    /* fall through（0x089c0bc5 → 0x089c0bde） */
                case 0x3145:                       // [waiting motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[waiting motion]", 0x3145));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str04 = val;
                        break;
                    }
                    /* fall through（→ 0x089c0c69） */
                case 0x3149:                       // [move motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[move motion]", 0x3149));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str08 = val;
                        break;
                    }
                    /* fall through */
                case 0x314d:                       // [sit motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[sit motion]", 0x314d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str0c = val;
                        break;
                    }
                    /* fall through */
                case 0x3151:                       // [damage motion 1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[damage motion 1]", 0x3151));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr10[0] = val;
                        break;
                    }
                    /* fall through */
                case 0x3155:                       // [damage motion 2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[damage motion 2]", 0x3155));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr10[1] = val;
                        break;
                    }
                    /* fall through */
                case 0x3159:                       // [down motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[down motion]", 0x3159));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str18 = val;
                        break;
                    }
                    /* fall through */
                case 0x315d:                       // [overturn motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[overturn motion]", 0x315d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str1c = val;
                        break;
                    }
                    /* fall through */
                case 0x3161:                       // [jump motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[jump motion]", 0x3161));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str20 = val;
                        break;
                    }
                    /* fall through */
                case 0x3165:                       // [jumpattack motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[jumpattack motion]", 0x3165));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str24 = val;
                        break;
                    }
                    /* fall through */
                case 0x3169:                       // [rest motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[rest motion]", 0x3169));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str28 = val;
                        break;
                    }
                    /* fall through */
                case 0x316d:                       // [throw motion 1-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 1-1]", 0x316d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[0] = val;
                        break;
                    }
                    /* fall through（0x089c11a7 → 0x089c11d7） */
                case 0x3171:                       // [throw motion 1-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 1-2]", 0x3171));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[0] = val;
                        break;
                    }
                    /* fall through（0x089c1232 → 0x089c1262） */
                case 0x3175:                       // [throw motion 2-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 2-1]", 0x3175));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[1] = val;
                        break;
                    }
                    /* fall through */
                case 0x3179:                       // [throw motion 2-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 2-2]", 0x3179));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[1] = val;
                        break;
                    }
                    /* fall through */
                case 0x317d:                       // [throw motion 3-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 3-1]", 0x317d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[2] = val;
                        break;
                    }
                    /* fall through */
                case 0x3181:                       // [throw motion 3-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 3-2]", 0x3181));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[2] = val;
                        break;
                    }
                    /* fall through */
                case 0x3185:                       // [throw motion 4-1]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 4-1]", 0x3185));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr2c[3] = val;
                        break;
                    }
                    /* fall through */
                case 0x3189:                       // [throw motion 4-2]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[throw motion 4-2]", 0x3189));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_arr3c[3] = val;
                        break;
                    }
                    /* fall through */
                case 0x318d:                       // [dash motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[dash motion]", 0x318d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str4c = val;
                        break;
                    }
                    /* fall through */
                case 0x3191:                       // [dashattack motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[dashattack motion]", 0x3191));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str50 = val;
                        break;
                    }
                    /* fall through */
                case 0x3195:                       // [getitem motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[getitem motion]", 0x3195));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str54 = val;
                        break;
                    }
                    /* fall through */
                case 0x3199:                       // [buff motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[buff motion]", 0x3199));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str58 = val;
                        break;
                    }
                    /* fall through */
                case 0x319d:                       // [back motion]
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[back motion]", 0x319d));
                    }
                    else
                    {
                        ScanStr(&val);
                        entry.m_str5c = val;
                        break;
                    }
                    /* fall through（0x089c182b → 0x089c185b） */
                case 0x31a1:                       // [attack motion]（多行）
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[attack motion]", 0x31a1));
                    }
                    else
                    {
                        while (ScanStr(&val))
                            entry.m_vec64.push_back(std::string(val.c_str()));
                        break;
                    }
                    /* fall through（0x089c18ba → 0x089c1980） */
                case 0x31a7:                       // [etc motion]（多行）
                    if (!__inited)
                    {
                        __lookup.insert(
                            AniLookupMap::value_type("[etc motion]", 0x31a7));
                    }
                    else
                    {
                        while (ScanStr(&val))
                            entry.m_vec70.push_back(std::string(val.c_str()));
                        break;
                    }
                    /* fall through（注册链尾 → 公共尾 0x089c1aa0） */
                default:
                    break;                     // 未命中（id=-1）静默
                }

                // [P292] ORIG 0x089c1ab0 `movzbl __inited; xor $0x1,%eax;
                // test %al,%al; je <跳出>` ⇒ 原码是 `if (!__inited) { …; continue; }`
                // 后接裸 break，非 `if (__inited) break;`（后者无 xor 物化）。
                if (!__inited)
                {
                    __init_lock.leave();       // 0x089c1ab5
                    __inited = true;           // 0x089c1ac1
                    continue;                  // → guard 重查（0x089c07f8）
                }
                break;                         // 解析完成 → 下一 ScanType
            }
        }

        table->insert(std::make_pair(fit->c_str(), entry));
    }
    return true;
