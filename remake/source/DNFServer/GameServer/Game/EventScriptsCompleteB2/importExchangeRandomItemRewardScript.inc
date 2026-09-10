bool importExchangeRandomItemRewardScript(ExchangeRandomItemRewardScript* script,
                                          char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;

    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_done;

        for (;;)
        {
            static LookupMap __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;

            int key;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                key = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();                        // +0x229
                if (__inited)
                {
                    __init_lock.leave();                    // +0x240
                    continue;   // +0x246 → guard 重查
                }
                key = 0;                                    // +0x24b 注册链入口
            }

            switch (key)
            {
            case 0x2e8:                                     // [/event]（+0x2a8）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x2e8));
                }
                else
                {
                    return true;                            // +0x305 mov $1,%ebx
                }
            }
            /* fall through */
            case 0x2ea:                                     // [title]（+0x30f）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x2ea));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x2ed:                                     // [explain]（+0x376）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x2ed));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x2f0:                                     // [material]（+0x3d7）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[material]", 0x2f0));
                }
                else
                {
                    if (!ScanType(token, true))
                        return false;                       // +0x445 直接返回假
                    if (token == "[exchange rule]")          // +0x460 串比较（非查表）
                    {
                        script->m_flag08 = static_cast<unsigned char>(
                            ScanInt(&ok));
                        // 规则读失败：跳过整数对循环直回公共尾（+0x48b→+0x636）。
                        if (!ok)
                            break;
                    }
                    // 非规则 token 与规则读取成功两路汇入同一整数对循环
                    // （+0x467 je +0x491 / +0x48b fall-through，ORIG 物理单份）。
                    for (;;)
                    {
                        unsigned int first  = 0;
                        unsigned int second = 0;
                        first = ScanInt(&ok);
                        if (!ok)
                            break;
                        second = ScanInt(&ok);
                        if (!ok)
                            break;
                        // 引用对临时（-0x4c）+ pair&lt;int,int&gt; 转换临时（-0x54）均为
                        // 内联实参 sret 槽（P134），不命名局部。
                        script->m_pairs0c.push_back(
                            std::pair<int, int>(std::make_pair(first, second)));
                    }
                    break;
                }
            }
            /* fall through */
            case 0x308:                                     // [random reward item]（+0x540）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[random reward item]", 0x308));
                }
                else
                {
                    for (;;)
                    {
                        int v = 0;                          // -0xcc 每轮清 0
                        v = ScanInt(&ok);
                        if (!ok)
                            break;
                        script->m_ints18.push_back(v);      // +0x18 vector<int>
                    }
                    break;
                }
            }
            /* fall through */
            case 0x312:                                     // [send mail]（+0x5d7）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send mail]", 0x312));
                }
                else
                {
                    importActionMailScript(&script->m_mail);   // +0x24
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x64b
                __inited = true;                            // +0x650
            }
            else
            {
                break;
            }
                continue;   // +0x657 → guard 重派发
        }
    }
scan_done:
    return ok;
