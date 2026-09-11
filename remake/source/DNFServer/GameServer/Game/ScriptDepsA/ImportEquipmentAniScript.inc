// ORIG ImportEquipmentAniScript T 0x089c05f8（报告 ## 汇编 + ## 反编译 C 双段
// 全文转写）。主文件列子文件名 → 逐个 loadRDARScriptFile 解析 →
// insert(make_pair(文件名 c_str(), 条目))（0x089c1acc 段：c_str →
// make_pair<char const*,STEquipmentAniScript&> → pair 转换 → map::insert）。
// 标签分派 = 函数级静态三件套（ORIG：__lookup map<char const*,int,less_TCHAR>
// guard 构造 0x089c07f8 / __init_lock SpinLock 0x089c0872 / __inited
// 0x094fde04）：未建表时持锁以 id=0 进入 switch，case 0 与 case 0x3142 合并
// （0x089c0a21 `test %eax,%eax; je 0x089c0b6a`），沿 case 排布逐例
// fall-through 注册 (tag,id) 共 26 例（0x089c0b6a LAYER → 0x089c0bde waiting
// → 0x089c0c69 move → 0x089c0cf4 sit → 0x089c0d7f dmg1 → 0x089c0e0a dmg2 →
// 0x089c0e95 down → 0x089c0f20 overturn → 0x089c0fab jump → 0x089c1036
// jumpattack → 0x089c10c1 rest → 0x089c114c throw1-1 → 0x089c11d7 throw1-2 →
// 0x089c1262 throw2-1 → 0x089c12ed throw2-2 → 0x089c1378 throw3-1 →
// 0x089c1403 throw3-2 → 0x089c148e throw4-1 → 0x089c1519 throw4-2 →
// 0x089c15a4 dash → 0x089c162f dashattack → 0x089c16ba getitem →
// 0x089c1745 buff → 0x089c17d0 back → 0x089c185b attack → 0x089c1980 etc），
// 链尾公共块 0x089c1aa0 释放锁、置 __inited 并 jmp 0x089c07f8 用当前 token
// 重查派发。字段映射（parse 分支 `lea -0x3f4; add $off` 实证）：
// throw n-1 → +0x2c+4*(n-1)、throw n-2 → +0x3c+4*(n-1)；attack/etc motion
// 为多行 vector（0x089c18bf/0x089c19e4 循环 string(val.c_str()) 临时 +
// push_back）。ORIG 尾 `mov %ebx,%eax` 返回 0/1（P159 判据 → int）。
// 残余 DIFF 归因（N 类豁免，语义全量恢复后 1223/1279）：
//  ① 主循环与内层 while 的 4.1 “xor$1;test;jne exit-island(nop;jmp)” 镜像
//     布局（0x089c0687..0693 等）= 负例“空 else nop;jmp 岛 vs
//     jump-threading”；② 子文件收集/attack/etc 循环的 EH landing pad 与
//     4.4 合并差异（0x089c0721/1b52 区，负例“new-expression landing-pad /
//     EH 双份 dtor epilogue vs merge”）；③ 4.1 vector::begin() 以 sret 返回
//     迭代器（0x089c0745 -0x3fc/-0x3f8 双槽）而 4.4 寄存器直返，帧 0x41c vs
//     0x40c、上方槽整体 -4 平移（P31/P112 结构性伪影）；④ 其余为 .o 重定位
//     地址显示差（链接 AE 口径消除）。
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
}