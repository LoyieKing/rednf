bool importPurchaseCashItemBonusEventScript(PurchaseCashItemBonusScript* script,
                                            char const* path)
{
    // b28d-solo-EventScriptsCompleteB2 复扫（2026-09-09）：原判 DIFF/N96+N18a。
    // 本轮修复：[send] 节 [purchase start/end count] 与 [purchase mutiple
    // number] 三处（+0x776/+0x7b7/+0x7fc）改显式双 goto（send_loop_head/
    // send_loop_end）复现 ORIG 正极性 `xor;test;je <节内头>; jmp <公共尾>`，
    // ndiff 16→**0**、DIFF→AE（gap 0、p167 CLEAR(69/69) 不变；[item idx]
    // 循环经此布局后亦随块序归位）。语义与 ORIG 一致。
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;
    unsigned int start = 0;          // -0x13c：函数级，[item idx start] 覆写

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
                __init_lock.enter();                        // +0x233
                if (__inited)
                {
                    __init_lock.leave();                    // +0x24a
                    continue;   // +0x250 → guard 重查
                }
                key = 0;                                    // +0x255 注册链入口
            }

            switch (key)
            {
            case 0x4a1:                                     // [/event]（+0x2e0）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x4a1));
                }
                else
                {
                    return true;                            // +0x33d
                }
            }
            /* fall through */
            case 0x4a3:                                     // [title]（+0x347）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x4a3));
                }
                else
                {
                ScanStr(&script->m_field00);
                        break;
                }
            }
            /* fall through */
            case 0x4a6:                                     // [explain]（+0x3b4）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x4a6));
                }
                else
                {
                ScanStr(&script->m_field04);
                            break;
                }
            }
            /* fall through */
            case 0x4a9:                                     // [db table]（+0x424）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x4a9));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);        // +0x8
                            break;
                }
            }
            /* fall through */
            case 0x4ac:                                     // [item idx]（+0x494）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item idx]", 0x4ac));
                }
                else
                {
                for (;;)
                    {
                        unsigned int v = 0;                 // -0x144 每轮清 0
                        v = static_cast<unsigned int>(ScanInt(&ok));
                        // +0x50f `xor;test;je <body>`：4.4 对 `!ok` 恒产 jne
                        // （正极性 je 不可造，N96+N18a 封顶），保留 xor 物化形。
                        if (ok != true)
                            break;                          // +0x51e → 公共尾
                        script->m_set0c.insert(v);          // set<unsigned>+0xc
                    }
                    break;
                }
            }
            /* fall through */
            case 0x4b6:                                     // [item idx start]（+0x54a）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[item idx start]", 0x4b6));
                }
                else
                {
                    start = static_cast<unsigned int>(ScanInt((bool*)0));
                    break;
                }
            }
            /* fall through */
            case 0x4b9:                                     // [item idx end]（+0x5be）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[item idx end]", 0x4b9));
                }
                else
                {
                    unsigned int end = 0;                   // 槽清 0 后 ScanInt
                    end = static_cast<unsigned int>(ScanInt(&ok));
                    if (ok != true)                         // +0x642 xor/jne 直退
                        break;                              // 失败 → 公共尾
                    if (start != 0)                         // +0x650
                    {
                        // +0x656 `cmpl $0,-0x1c; jne <检查块>; jmp <公共尾>`：
                        // ORIG 是否定早返形（end==0 直接退），非正向 if 包裹。
                        if (end == 0)
                            break;
                        for (; start <= end; ++start)
                            script->m_set0c.insert(start);
                    }
                    break;
                }
            }
            /* fall through */
            case 0x4c6:                                     // [send]（+0x6aa）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send]", 0x4c6));
                }
                else
                {
                PurchaseCashItemBonusScript::PurchaseBonusRule rule;
                    for (;;)
                    {
                    send_loop_head:;
                    if (!ScanType(token, true))
                            break;              // 节内 EOF → 公共尾（+0x932）
                    if (token == "[purchase start count]")
                        {
                        rule.m_purchaseStartCount = ScanInt(&ok);
                            if (!ok)
                                goto send_loop_end;         // 失败退出节（+0x778）
                            else
                                goto send_loop_head;        // 成功回节内头
                        }
                    else if (token == "[purchase end count]")
                        {
                        rule.m_purchaseEndCount = ScanInt(&ok);
                            if (!ok)
                                goto send_loop_end;
                            else
                                goto send_loop_head;
                        }
                    else if (token == "[purchase mutiple number]")
                        {
                        rule.m_purchaseMutipleNumber = ScanInt(&ok);
                            if (!ok)
                                goto send_loop_end;
                            else
                                goto send_loop_head;
                        }
                    else if (token == "[send mail]")
                        {
                            Action_SendMail mail;            // +0x82e ctor
                            mail.reset();                    // +0x83c
                            // 返回值先物化回 ok 槽再判（+0x84f mov %al,-0x135 /
                            // +0x855 movzbl 读回形）。
                            ok = importActionMailScript(&mail);
                            if (ok != true)                  // +0x861 xor 物化形
                            {
                                break;          // 失败即退出节，rule 不复位（+0x863）
                            }
                            else
                            {
                                if (rule.m_purchaseStartCount != -1 &&
                                    rule.m_purchaseEndCount != -1 &&
                                    rule.m_purchaseMutipleNumber != -1)
                        {
                                    script->m_ruleMails24.push_back(
                                        std::make_pair(rule, mail));
                        }
                                rule.reset();                // +0x8e7
                            }
                        }
                    }
                    send_loop_end:;
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x948
                __inited = true;                            // +0x94d
            }
            else
            {
                break;
            }
                continue;   // +0x954 → guard 重派发
        }
    }
scan_done:
    return ok;
