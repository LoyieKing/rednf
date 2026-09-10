bool importUsedFatigueGiveItemEventScript(UsedFatigueGiveItemScript* script,
                                          char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164/P177 同族）
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = true;

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
                case 0x58f:                                  // [/event]（+0x2ec）
                case 0:
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/event]", 0x58f));
                    }
                    else
                    {
                        return result;                      // +0x349 movzbl -0xf9
                    }
                }
                /* fall through */
                case 0x592:                                  // [title]（+0x355）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[title]", 0x592));
                    }
                    else
                    {
                        ScanStr(&script->m_field00);
                        break;
                    }
                }
                /* fall through */
                case 0x595:                                  // [explain]（+0x3c2）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[explain]", 0x595));
                    }
                    else
                    {
                        ScanStr(&script->m_field04);
                        break;
                    }
                }
                /* fall through */
                case 0x598:                                  // [db table]（+0x432）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[db table]", 0x598));
                    }
                    else
                    {
                        ScanStr(&script->m_dbTableName);
                        break;
                    }
                }
                /* fall through */
                case 0x59c:                                  // [user type]（+0x4a2）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[user type]", 0x59c));
                    }
                    else
                    {
                        script->m_userType =
                            static_cast<unsigned char>(ScanInt(&result));
                        if (!result)
                            return false;                   // +0x527
                        break;
                    }
                }
                /* fall through */
                case 0x5a1:                                  // [used fatigue]（+0x531）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[used fatigue]", 0x5a1));
                    }
                    else
                    {
                        script->m_usedFatigue = ScanInt(&result);
                        if (!result)
                            return false;                   // +0x5a5
                        break;
                    }
                }
                /* fall through */
                case 0x5a7:                                  // [charc event]（+0x5af）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[charc event]", 0x5a7));
                    }
                    else
                    {
                        script->m_charcEventFlag = 1;       // +0x600 movb $1
                        break;
                    }
                }
                /* fall through */
                case 0x5aa:                                  // [apply job type]（+0x609）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[apply job type]", 0x5aa));
                    }
                    else
                    {
                        std::string jobName;                // +0x660 局部 ctor
                        ScanStr(&jobName);
                        script->m_jobType =
                            getCharacterJobToEnumValue(jobName);
                        break;                              // +0x695 dtor → 公共尾
                    }
                }
                /* fall through */
                case 0x5af:                                  // [used fatigue limit]（+0x6b5）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[used fatigue limit]", 0x5af));
                    }
                    else
                    {
                        script->m_fatigueLimit = ScanInt(&result);
                        if (!result)
                            return false;                   // +0x725
                        break;
                    }
                }
                /* fall through */
                case 0x5b5:                                  // [send mail]（+0x72f）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[send mail]", 0x5b5));
                    }
                    else
                    {
                        // 失败仅置假 result 并继续主循环（+0x792 movb $0,-0xf9）。
                        if (!importActionMailScript(&script->m_mail))
                            result = false;
                        break;
                    }
                }
                }

                if (!__inited)
                {
                    __init_lock.leave();                    // +0x7b7
                    __inited = true;                        // +0x7bc
                }
                else
                {
                    break;
                }
                    continue;   // +0x7c3 → guard 重派发
        }
    }
scan_done:
    return result;
