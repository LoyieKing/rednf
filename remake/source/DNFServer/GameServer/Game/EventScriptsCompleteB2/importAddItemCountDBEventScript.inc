bool importAddItemCountDBEventScript(AddItemCountDBScript* script, char const* path)
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
            case 0xe9:                                      // [/event]（+0x2a8）
            case 0:
            {
                if (!__inited)
        {
                    __lookup.insert(LookupMap::value_type("[/event]", 0xe9));
            }
                else
            {
                    return true;                            // +0x305
                }
            }
            /* fall through */
            case 0xeb:                                      // [title]（+0x30f）
            {
                if (!__inited)
            {
                    __lookup.insert(LookupMap::value_type("[title]", 0xeb));
                }
                else
                {
                ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0xee:                                      // [explain]（+0x36d）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0xee));
                }
                else
                {
                ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0xf1:                                      // [db table]（+0x3ce）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0xf1));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);       // +0x20
                    break;
                }
            }
            /* fall through */
            case 0xf4:                                      // [item index]（+0x42f）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item index]", 0xf4));
                }
                else
                {
                    for (;;)
                    {
                        int v = 0;                          // -0xb4 每轮清 0
                        v = ScanInt(&ok);
                        if (ok)
                            script->m_itemIdxs.push_back(v);   // +0x8
                        else
                            break;                          // +0x4a4 → 公共尾
                    }
                    break;
                }
            }
            /* fall through */
            case 0xff:                                      // [quest index]（+0x4c4）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[quest index]", 0xff));
                }
                else
                {
                    for (;;)
                    {
                        int v = 0;                          // -0xb8 每轮清 0
                        v = ScanInt(&ok);
                        if (ok)
                            script->m_questIds.push_back(v);   // +0x14
                        else
                            break;                          // +0x539 → 公共尾
                    }
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x56e
                __inited = true;                            // +0x573
            }
            else
            {
                break;
            }
                continue;   // +0x57a → guard 重派发
        }
    }
scan_done:
    return ok;
