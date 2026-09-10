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
