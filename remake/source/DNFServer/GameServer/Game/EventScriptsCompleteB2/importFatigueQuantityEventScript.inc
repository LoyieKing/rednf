bool importFatigueQuantityEventScript(FatigueQuantityScript* script, char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3；ORIG 0x08981539
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;

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
                __init_lock.enter();                        // +0x226
                if (__inited)
                {
                    __init_lock.leave();                    // +0x23d
                    continue;   // +0x243 → guard 重查
                }
                key = 0;                                    // +0x248 注册链入口
            }

            switch (key)
            {
            case 0x478:                                     // [/event]
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x478));
                }
                else
                {
                    return true;
                }
            }
            /* fall through */
            case 0x47a:                                     // [title]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x47a));
                }
                else
                {
                    ScanStr(&script->m_field00);
                    break;
                }
            }
            /* fall through */
            case 0x47d:                                     // [explain]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x47d));
                }
                else
                {
                    ScanStr(&script->m_field04);
                    break;
                }
            }
            /* fall through */
            case 0x480:                                     // [db table]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[db table]", 0x480));
                }
                else
                {
                    ScanStr(&script->m_dbTableName);
                    break;
                }
            }
            /* fall through */
            case 0x483:                                     // [user type]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[user type]", 0x483));
                }
                else
                {
                    script->m_userType =
                        static_cast<unsigned char>(ScanInt((bool*)0));
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x489
                __inited = true;                            // +0x48e
            }
            else
            {
                break;
            }
                continue;   // +0x495 → guard 重派发
        }
    }
scan_done:
    return result;
