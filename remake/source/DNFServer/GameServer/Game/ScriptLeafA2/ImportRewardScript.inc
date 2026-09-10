int eventReward::ImportRewardScript(const char* dir, const char* file)
{
    // [b28d-solo-ScriptLeafA2] 结构按 ORIG 0x080ef3a2 还原为 **P333 状态机形**：
    //   每 case 块头 `if (__inited) {业务; break;} else {insert; fallthrough}`，
    //   9 项插完后共同尾部 `if(!__inited){leave; __inited=1; goto retry;}` 回跳重查
    //   （ORIG 0x080f01d5/0x080f01e3）；static 三件套声明在循环体内 retry 标签之后
    //   （P177：每轮穿 guard，ORIG 0x080ef438 每轮执行 guard 检查）。
    //   业务块逐块按报告 ## 汇编 转写，含 3 处此前漏掉的返回值检查：
    //   ①[job type] ScanStr 失败 → return 0（0x080ef79a）；②[sub type] 首 ScanStr
    //   失败 → return 0（0x080ef8d9）；③[option] ScanStr 成功才调 getOptionType
    //   （0x080efa47 失败直跳析构）。[equipment item]/[stackable item] 行扫描失败
    //   一律回主循环（0x080efb6b/0x080efb92/0x080efe03/0x080efe2a/0x080efe51 →
    //   0x080f01d5），非 return 0；[stackable item] 的 `curItem.m_count = 1` 在
    //   循环体内每轮执行（0x080efdb3）。key 三标量为 signed char 直落（0x080ef7be）。
    //   读数（本波 fresh）：gap 30→18（ORIG 885 vs 我方 867）；p167 **122=122**
    //   （EXTRA/MISSING 仅剩 C1↔C2/D1↔D2 对称差；itemDataD1Ev ×6 已由 itemData
    //   删用户空析构消除）。残余 18 条 = 每 call 段 ≤2 条的槽/寄存器形差
    //   （N82/N18a）；判**语义完成、byte 层封顶**。
    if (!loadRDARScriptFile(dir, file))
    {
        return 0;
    }

    std::string token;                              // -0x188
    bool ok = false;                                // -0x189
    std::map<int, std::vector<itemData> > subMap;   // -0x1a4
    rewardKey key;                                  // -0x1a9
    int deadPad = 0;                                // -0x20（ORIG 死局部，P232）
    itemData curItem;                               // -0x1b8
    (void)deadPad;

    while (ScanType(token, true))
    {
    retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static char __inited = 0;
        int id;

        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(token.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            id = 0;                                 // 首遍登记态
        }

        switch (id)
        {
        case 0:         // 首轮哨兵（与 [/reward] 共体）
        case 0x8f:      // [/reward]
            // [A16 复核] ORIG 0x080ef6b8 段 `movzbl __inited; xor $1; test %al; je`
            // 不可复现：把本处改写为否定形 `if(!__inited){insert}else{return 1;}`
            // 实测 4.4 产 `test;jne`（**无 xor**）且 +1 条 ⇒ 回退。4.4 对静态
            // bool/char 的 `!` 恒折 `test;jne`（N122 仅覆盖具名局部），
            // 与 4.1 的 xor 物化互斥 ⇒ 12 处检查点差记 N168a/N122 封顶。
            if (__inited)
            {
                return 1;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/reward]", 0x8f));
            // fallthrough
        case 0x93:      // [job type]
            if (__inited)
            {
                std::string tok2;
                if (!ScanStr(&tok2))
                {
                    return 0;
                }
                key.m_job = static_cast<signed char>(getCharacterJobToEnumValue(tok2));
                key.m_a = static_cast<signed char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                key.m_b = static_cast<signed char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0;
                }
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[job type]", 0x93));
            // fallthrough
        case 0xa3:      // [sub type]
            if (__inited)
            {
                std::string tok2;
                if (!ScanStr(&tok2))
                {
                    return 0;
                }
                key.m_sub.m_type = static_cast<signed char>(getSubKeyType(tok2));
                std::string tok3;
                if (!ScanStr(&tok3))
                {
                    return 0;
                }
                key.m_sub.m_action = static_cast<signed char>(getSubKeyAction(tok3));
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[sub type]", 0xa3));
            // fallthrough
        case 0xaf:      // [option]
            if (__inited)
            {
                std::string tok2;
                if (ScanStr(&tok2))
                {
                    getOptionType(tok2, curItem);
                }
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[option]", 0xaf));
            // fallthrough
        case 0xb9:      // [equipment item]
            if (__inited)
            {
                defaultOption(key.m_sub.m_action, curItem);
                for (;;)
                {
                    int findKey = 0;
                    itemData item = curItem;        // 栈副本
                    findKey = ScanInt(&ok);         // ORIG 键为独立局部 int（不占 itemData）
                    if (!ok)
                    {
                        break;                      // 回主循环
                    }
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                      // 回主循环（非 return 0）
                    }
                    std::map<int, std::vector<itemData> >::iterator fit =
                        subMap.find(findKey);
                    if (fit != subMap.end())
                    {
                        fit->second.push_back(item);
                    }
                    else
                    {
                        std::vector<itemData> vec;
                        vec.push_back(item);
                        subMap.insert(std::make_pair(findKey, vec));
                    }
                }
                break;
            }
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                "[equipment item]", 0xb9));
            // fallthrough
        case 0xd8:      // [stackable item]
            if (__inited)
            {
                defaultOption(key.m_sub.m_action, curItem);
                for (;;)
                {
                    int findKey = 0;
                    curItem.m_count = 1;            // 每轮重置（ORIG 0x080efdb3）
                    itemData item = curItem;
                    findKey = ScanInt(&ok);         // ORIG 键为独立局部 int
                    if (!ok)
                    {
                        break;                      // 回主循环
                    }
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                      // 回主循环（非 return 0）
                    }
                    item.m_count = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                      // 回主循环（非 return 0）
                    }
                    std::map<int, std::vector<itemData> >::iterator fit =
                        subMap.find(findKey);
                    if (fit != subMap.end())
                    {
                        fit->second.push_back(item);
                    }
                    else
                    {
                        std::vector<itemData> vec;
                        vec.push_back(item);
                        subMap.insert(std::make_pair(findKey, vec));
                    }
                }
                break;
            }
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type(
                "[stackable item]", 0xd8));
            // fallthrough
        case 0xfc:      // [/sub type]
            if (__inited)
            {
                m_map0.insert(std::make_pair(key, subMap));
                key.m_sub.clear();                  // rewardSubKey::clear 仅清子键
                subMap.clear();
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/sub type]", 0xfc));
            // fallthrough
        case 0x102:     // [/job type]
            if (__inited)
            {
                key.clear();                        // rewardKey::clear
                subMap.clear();
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/job type]", 0x102));
            // fallthrough
        case 0x107:     // [/option]
            if (__inited)
            {
                curItem.clear();                    // itemData::clear（W 0x080f1162，call 实证）
                break;
            }
            __lookup.insert(
                std::map<const char*, int, less_TCHAR>::value_type("[/option]", 0x107));
            break;

        default:        // 未知令牌忽略
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = 1;
            goto retry;
        }
    }
    // 自然 EOF：返回最后一次 ok 态（ORIG ebx=local_18d）
    return ok ? 1 : 0;
