// ============================================================================
// importPcBangEquipmentRendar — ORIG 0x08915606（init.md +0x6be8，
// "Etc/PcBangRentalService.etc" → m_pcBangRentar）。
// "[section]"：EventRewardSection 三字节 + count 个 uchar 入 m_vec0；
// "[group]"：job/grade→key，收集 "[package selection]" 包（itemId、count、
// short、"逗号串"→tokenize/getGrowTypeIndex 逐项 push_back），[/group] 时
// insert 进 m_mapC。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N78/N7/N104] ORIG 0x08915606
//  逐 jcc/调用点核对：无语义缺口。jcc 27:27 一一对应，唯一极性差 = [section]
//  do-while 的 more 标志（ORIG 0x089159e7 `test %ebx,%ebx; je` 寄存器化 vs
//  我方 `cmpb $0,-0x29(%ebp); jne` 槽化，do-while 语义等价）；内层 items 循环
//  4.4 旋转（条件落尾 0x2b46 vs ORIG 落头 0x08915b02）；growText/delim 临时
//  析构的 EH 双份 epilogue vs 4.1 merge（N104）。
//  p167 别名口径 CLEAR（MISSING C2/D2 ↔ EXTRA C1/D1 一一配对，P93）；
//  无 call *（无虚调用）；orig/ours=582/581。
// [F2e-Singles2 C05② 复核 2026-09-10] cmp 差 2 处：①0x08915cb9 `cmp -0x1c(%ebp),%eax;
//  setg` —— 左操作数是**跨调用返回值**（0x08915cb4 call 结果直用）⇒ 边界表 ❌；
//  ②0x08915998 `cmp -0x30(%ebp),%eax; setl` 我方 -0x34 槽（槽差，属 N82 平移）。
//  ⇒ 非 C05② 命中形，维持 N78/N7/N104 封顶（our 581 vs ORIG 582，gap +1）。
bool importPcBangEquipmentRendar(const char* path, PcBangItemRentarData* out)
{
    // ORIG 双相派发（T 0x08915606）：out 判空先于 loadRDAR（+0x0c）；__lookup
    // 三件套位于主循环体内；注册链 [section]0x33f6→[group]0x3406+?（0x3409）。
    // [section] 执行形 = do-while：一次 token 命中后连续解析多节直至
    // ScanInt(&ok) 断流（**bVar2=false 仅退出该 do-while 回主循环，非 return**）；
    // [group] 执行形 = 内层 ScanType 循环：[package selection] 组 items，
    // [/group] 提交 make_pair+map insert，内层 EOF 无提交直接回主循环。
    // 收敛口径：orig=582 ours=581，语义/槽位/调用序已逐段对齐；残余 1 条及
    // 零散助记符差为跨编译器形态：[section] do-while 的 more 标志 4.1 全程走
    // %ebx（本侧 bool 槽 spill，N78 register-bool 寄存器选择不控）、
    // 内层 items 循环 4.4 旋转（scan 落底 vs ORIG 落头，穷举 for/while/
    // while(true)+break 三形均不能同时保住 xor$1 物化与头置）、growText/
    // delim/tokenize 临时析构的 EH 双份 epilogue vs 4.1 merge（N 在册）。
    if (out == NULL)
    {
        return false;
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                    // ORIG -0xa4
    std::string nameText;                 // ORIG -0xa8
    bool ok = false;                      // ORIG -0xa9
    int count = 0;                        // ORIG -0x30（prologue 初始化，跨段复用）

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x33f6:                   // [section]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[section]", 0x33f6));
                }
                else
                {
                    bool more = false;
                    do
                    {
                        EventRewardSection section;   // 每轮构造/析构（ORIG -0xc0）
                        section.m_field0 = ScanInt(&ok);
                        if (ok == true)
                        {
                            section.m_field1 = ScanInt((bool*)NULL);
                            section.m_field2 = ScanInt((bool*)NULL);
                            count = ScanInt((bool*)NULL);
                            for (int i = 0; i < count; ++i)
                            {
                                // ORIG 0x08915c??：push_back(EOh) 右值形
                                //（p167：MISSING push_back(EOh) / EXTRA (RKh)）
                                section.m_items.push_back(
                                    static_cast<unsigned char>(ScanInt((bool*)NULL)));
                            }
                            out->m_vec0.push_back(section);
                            more = true;
                        }
                        else
                        {
                            more = false;
                        }
                    } while (more);
                    break;
                }
                /* fall through（ORIG +0x2e9 jmp +0x3ee） */
            case 0x3409:                   // [group]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[group]", 0x3409));
                }
                else
                {
                    if (!ScanStr(&nameText))
                        return false;      // ORIG +0x454 ebx=0 → 直接出口 return false
                    int job = getCharacterJob(nameText.c_str());
                    int grade = ScanInt((bool*)NULL);
                    int key = makeFairPVPEquipmentPackageKey(job, grade);
                    std::vector<std::vector<EventRewardItemInfo> > groups;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                            goto group_no_submit;   // ORIG +0x4ca→+0x850 EOF 无提交
                        if (token == "[package selection]")
                        {
                            std::vector<EventRewardItemInfo> items;
                            int itemIdx;
                            while (ScanInt(&itemIdx) == 1)
                            {
                                EventRewardItemInfo item;
                                item.m_field0 = itemIdx;
                                item.m_short8 = ScanInt((bool*)NULL);
                                item.m_field4 = ScanInt((bool*)NULL);
                                ScanStr(&nameText);
                                std::string growText(nameText.c_str());
                                std::vector<std::string> parts;
                                std::string delim(",");
                                tokenizeStr(growText, parts, delim);
                                unsigned int p = 0;
                                while ((int)parts.size() > (int)p)
                                {
                                    int grow = getGrowTypeIndex(parts.at(p).c_str());
                                    item.m_growTypes.push_back(grow);
                                    p = p + 1;
                                }
                                items.push_back(item);
                            }
                            groups.push_back(items);
                            continue;
                        }
                        if (token == "[/group]")
                            break;
                    }
                    out->m_mapC.insert(std::make_pair(key, groups));
                group_no_submit:
                    ;
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_08915e80 公共尾。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return true;
}