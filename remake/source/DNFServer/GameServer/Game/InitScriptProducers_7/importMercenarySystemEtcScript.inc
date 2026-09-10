int importMercenarySystemEtcScript(const char* path, stMercenarySystemInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return 9;
    if (out == NULL)
        return 0xc;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                 // ORIG -0x11c
    std::string sub;                   // ORIG -0x120
    std::string sub2;                  // ORIG -0x124
    std::string unused;                // ORIG -0x128（构造/析构但零引用槽）
    bool ok = false;                   // ORIG -0x129
    short lv = 0;                      // ORIG -0x12c（movw $0；%ax 写）
    unsigned int gold = 0;             // ORIG -0x130
    unsigned int item = 0;             // ORIG -0x134
    unsigned int avatarIdx = 0;        // ORIG -0x138
    unsigned int period = 0;           // ORIG -0x13c
    float rate = 0;                    // ORIG -0x140
    unsigned int count = 0;            // ORIG -0x144
    unsigned int index = 0;            // ORIG -0x148

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        // ORIG 重派发跳回 +0xf0（__lookup guard）：静态声明位于主循环体内。
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
            case 0x25:                      // [base time unit]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[base time unit]", 0x25));
                }
                else
                {
                    period = ScanInt(&ok);
                    if (!ok) return 0x29;
                    if (period == 0) return 0x2c;
                    out->m_field0 = period;
                    break;
                }
                /* fall through（ORIG +0x383 jmp +0x3d5） */
            case 0x32:                      // [level base GoldItem Prob]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[level base GoldItem Prob]", 0x32));
                }
                else
                {
                    lv = ScanInt(&ok);
                    if (!ok) return 0x36;
                    gold = ScanInt(&ok);
                    if (!ok) return 0x3a;
                    item = ScanInt(&ok);
                    if (!ok) return 0x3e;
                    out->m_vec8.push_back(std::make_pair(lv, std::make_pair(gold, item)));
                    // [F2e-Singles2 轴B 2026-09-10] ORIG 089f7192 `movzwl 0x4(%eax),%edx;
                    // movzwl -0x12c(%ebp),%eax; cmp %ax,%dx; **jle**`（16 位**有符号**）vs
                    // 我方 `jbe`（无符号）⇒ 真符号性分叉（lv<0 时分叉；P196 家族）。
                    // 本轮试两形：① `(short)out->m_field4 > lv`、② `lv < (short)out->
                    // m_field4` —— 两形均复现 `cmp %ax,%dx; jle` ✓，但 4.4 对
                    // 16 位 signed 比较恒多一条中转 `movzwl 0x4(%eax),%eax; mov %eax,%edx`
                    // （ORIG 4.1 直落 `movzwl 0x4(%eax),%edx`，N18a 反向）⇒ our 838→**839**
                    // （+1）、diff 746→751（+5）。
                    // [Main 裁决 2026-09-10] 为消除 **signed/unsigned 语义分叉保留**（语义
                    // 对齐类不受 gap/ndiff 闸门约束）：`short lv` 可为负，signed 比较下
                    // `lv<0` 恒写回、unsigned 下恒不写回 ⇒ 行为相反。代价 +1 条属 N18a
                    // 反向（不可消除），该件本为 DIFF ⇒ 无 identical/verdict 回退风险。
                    if ((short)out->m_field4 > lv)
                        out->m_field4 = (unsigned short)lv;
                    break;
                }
                /* fall through（+0x430 jmp +0x55e） */
            case 0x46:                      // [mercenary reward per period]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward per period]", 0x46));
                }
                else
                {
                    while (true)
                    {
                        period = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x50;
                        out->m_vec14.push_back(std::make_pair(period, rate));
                    }
                    break;
                }
                /* fall through（+0x5b9 jmp +0x66c） */
            case 0x57:                      // [mercenary reward avatar bonus]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward avatar bonus]", 0x57));
                }
                else
                {
                    while (true)
                    {
                        avatarIdx = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x61;
                        out->m_vec20.push_back(std::make_pair(avatarIdx, rate));
                    }
                    break;
                }
                /* fall through（+0x6c7 jmp +0x774） */
            case 0x69:                      // [mercenary reward critical]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward critical]", 0x69));
                }
                else
                {
                    if (!ScanStr(&out->m_str2c))
                        return 0x6c;
                    out->m_field3c = 0;
                    while (true)
                    {
                        gold = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x77;
                        out->m_field3c += gold;      // ORIG 累计和
                        out->m_vec30.push_back(std::make_pair(gold, rate));
                    }
                    break;
                }
                /* fall through（+0x7c0 jmp +0x8a9） */
            case 0x7f:                      // [competition area]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[competition area]", 0x7f));
                }
                else
                {
                    stMercenaryCompetitionArea area;
                    int groupIdSum = 0;              // ORIG -0x1c
                    while (true)
                    {
                        if (!ScanType(sub, true))
                            break;
                        if (sub == "[reward group]")
                        {
                            stMercenaryRewardGroup group;
                            group.m_groupId = ScanInt(&ok);
                            groupIdSum += group.m_groupId;   // ORIG 先累加后检查 ok
                            if (!ok) return 0x8f;
                            if (!ScanStr(&group.m_name))
                                return 0x92;
                            while (true)
                            {
                                if (!ScanType(sub2, true))
                                    return 0x97;
                                if (sub2 == "[mob index]")
                                {
                                    index = ScanInt(&ok);
                                    if (!ok) continue;
                                    count = ScanInt(&ok);
                                    if (!ok) return 0xa3;
                                    group.m_field20 = group.m_field20 + count;   // group+0x20 累计（ORIG 两段物化）
                                    group.m_mobList.push_back(std::make_pair(count, index));
                                    continue;
                                }
                                if (sub2 == "[item index]")
                                {
                                    index = ScanInt(&ok);
                                    if (!ok) continue;
                                    count = ScanInt(&ok);
                                    if (!ok) return 0xb3;
                                    group.m_field24 = group.m_field24 + count;   // group+0x24 累计（ORIG 两段物化）
                                    group.m_itemList.push_back(std::make_pair(count, index));
                                    continue;
                                }
                                if (sub2 == "[/reward group]")
                                {
                                    area.m_groups.push_back(group);
                                    break;
                                }
                            }
                            continue;
                        }
                        if (sub == "[world map]")
                        {
                            area.m_worldMapIdx = ScanInt(&ok);  // ORIG 无 ok 检查
                            continue;
                        }
                        if (sub == "[area level]")
                        {
                            area.m_areaLevel = ScanInt(&ok);    // ORIG 无 ok 检查
                            continue;
                        }
                        if (sub == "[/competition area]")
                        {
                            area.m_groupIdSum = groupIdSum;
                            out->m_vec40.push_back(area);
                            break;
                        }
                        return 0xd1;
                    }
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_089f795a 公共尾：注册链走完释放锁置位重派发；正常态下一轮。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return 0;
