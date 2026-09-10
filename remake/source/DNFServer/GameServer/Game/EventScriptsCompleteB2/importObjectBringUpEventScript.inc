bool importObjectBringUpEventScript(ObjectBringUpScript* script, char const* path)
{
    // b28d-solo-EventScriptsCompleteB2 复扫（2026-09-09）：原判 DIFF/N96+N18a。
    // 本轮修复：[send] 节三处 reward 检查（+0x8aa/+0x8eb/+0x930）改用显式
    // `if(!result) goto send_loop_end; else goto send_loop_head;`（双非紧邻
    // goto）复现 ORIG 正极性 `xor;test;je <body>`，ndiff 20→4（gap 0、
    // p167 CLEAR(86/86) 不变）。残差 = [material item] idx 循环 1 处 je/jne
    // + 节尾 jmp 岛（4.4 jump-thread 实证不可翻）⇒ 语义完成、N 封顶。
    // 2026-09-10：[get eventitem fatigue] 两处内层后跳 `goto send_sec_head;`
    // 改「内层 break + 外层 continue」（消 entry jmp）⇒ **AE**（657/657、
    // clean 643/643、判据① 方向差 = 0）。
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;   // 兼作节内 ScanInt 的 ok（ORIG 同槽 -0x165）
    // P31 dead local（switch 前）：复现 ORIG 帧 0x1dc 与 mail/rule/
    // fatigues 深端槽号（-0x1cc/-0x188/-0x17c）。token/iterator 交错槽
    // 序（ORIG conv2 -0x158 vs 4.4 -0x168 区）属跨版本栈槽分配器差异
    // （P112/N76 族），无法由合法源码复现，残差按 N 类标注。

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
            case 0x509:                                     // [/event]（+0x2bf）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x509));
                }
                else
                {
                    return true;                            // +0x31c
                }
            }
            /* fall through */
            case 0x50b:                                     // [title]（+0x326）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x50b));
                }
                else
                {
                ScanStr(&script->m_field00);
                            break;
                }
            }
            /* fall through */
            case 0x50e:                                     // [explain]（+0x393）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x50e));
                }
                else
                {
                ScanStr(&script->m_field04);
                            break;
                }
            }
            /* fall through */
            case 0x511:                                     // [db table]（+0x403）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x511));
                }
                else
                {
                ScanStr(&script->m_dbTableName);
                            break;
                }
            }
            /* fall through */
            case 0x515:                                     // [material item]（+0x473）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[material item]", 0x515));
                }
                else
                {
                    if (!ScanType(token, true))
                        return false;                       // +0x4f0 直返假
                    if (token == "[exchange rule]")          // +0x50b 串直比
                    {
                    script->m_exchangeRuleFlag =
                        static_cast<unsigned char>(ScanInt(&result));
                        // 失败退出节回主循环（+0x536 jne 公共尾）。
                        if (result != true)
                            break;
                    }
                    // 两路汇入同一整数收集循环（物理单份，+0x53c）。
                    for (;;)
                    {
                        int idx = 0;                        // -0x1c 每轮清 0
                        idx = ScanInt(&result);
                        if (result != true)
                            break;                          // +0x563 → 公共尾
                        // ORIG +0x568：idx 复制到临时槽 -0xec 后绑右值引用
                        // （push_backEOi）⇒ 实参必须是右值表达式。
                        script->m_materialItemIdxs.push_back(static_cast<int>(idx));
                    }
                    break;
                }
            }
            /* fall through */
            case 0x529:                                     // [eventitem send]（+0x58b）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[eventitem send]", 0x529));
                }
                else
                {
                    std::vector<unsigned int> fatigues;     // -0x178 节局部
                    for (;;)
                    {
                send_sec_head:;
                    if (!ScanType(token, true))
                            break;              // EOF → 退节（+0x7af→+0x7b0）
                    if (token == "[get eventitem fatigue]")
                        {
                    for (;;)
    {
                                unsigned int f = 0;         // -0x17c 每轮清 0
                                f = static_cast<unsigned int>(ScanInt(&result));
                                if (result != true)
                                    break;                  // 失败回节内头（+0x664）
                                fatigues.push_back(f);
                        }
                        continue;                       // +0x664 回节内头
                        }
                    else if (token == "[send mail]")
                        {
                            Action_SendMail mail;           // -0x1cc ctor+reset
                        mail.reset();
                            result = importActionMailScript(&mail);
                            if (!result)
                                break;                      // 失败退节（+0x786）
                            script->m_ruleMails1c.push_back(
                                        std::make_pair(fatigues, mail));
                            fatigues.clear();               // +0x754 节内复位
                            continue;                       // +0x78a 回节内头
                        }
                    else if (token == "[/eventitem send]")
                            break;                          // +0x7ad 退节
                        // 未知 token：+0x7a7 回节内头继续
                else
                            continue;
                    }
                    break;
                }
            }
            /* fall through */
            case 0x54c:                                     // [send]（+0x626）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send]", 0x54c));
                }
                else
                {
                    ObjectBringUpScript::ObjectBringUpRule rule;   // -0x18c
                    for (;;)
                    {
                    send_loop_head:;
                    if (!ScanType(token, true))
                            break;                          // EOF → +0xa64 公共尾
                    if (token == "[reward start count]")
                        {
                        rule.m_rewardStartCount = ScanInt(&result);
                            if (!result)
                                goto send_loop_end;         // 失败 → 公共尾
                            else
                                goto send_loop_head;        // +0x8aa→节内头
                        }
                    else if (token == "[reward end count]")
                        {
                        rule.m_rewardEndCount = ScanInt(&result);
                            if (!result)
                                goto send_loop_end;
                            else
                                goto send_loop_head;
                        }
                    else if (token == "[reward mutiple number]")
                        {
                        rule.m_rewardMutipleNumber = ScanInt(&result);
                            if (!result)
                                goto send_loop_end;
                            else
                                goto send_loop_head;
                        }
                    else if (token == "[send mail]")
                        {
                            Action_SendMail mail;           // ctor + reset（+0x962/970）
                        mail.reset();
                            result = importActionMailScript(&mail);
                            if (!result)
                                break;           // 失败退节，rule 不复位（+0x997 直落）
                            if (rule.m_rewardStartCount != -1 &&
                                rule.m_rewardEndCount != -1 &&
                                rule.m_rewardMutipleNumber != -1)
                            {
                                script->m_sendMails28.push_back(
                                            std::make_pair(rule, mail));
                            }
                            rule.reset();               // 仅成功路径复位（+0xa24）
                            continue;                   // +0xa58 回节内头
                        }
                        // 未知 token：+0x953 回节内头继续
                else
                            continue;
                    }
                    send_loop_end:;
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0xa79
                __inited = true;                            // +0xa7e
            }
            else
            {
                break;
            }
                continue;   // +0xa85 → guard 重派发
        }
        }
scan_done:
    return result;
