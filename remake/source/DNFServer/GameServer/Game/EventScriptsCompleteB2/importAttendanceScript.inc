bool importAttendanceScript(AttendanceScript* script, char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3，P164 scan_exit 岛标签四函数族之一）
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
            // 三静态位于外层 token 循环体首；ORIG 重派发 jmp +0x66 = guard 检查点，
            // goto 边每轮穿过 guard 测试。__inited 零初始化无 guard。
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
                case 0x26f:                                  // [/event]（+0x306）
                case 0:
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/event]", 0x26f));
                    }
                    else
                    {
                        return true;                        // +0x363 mov $1,%ebx
                    }
                }
                /* fall through */
                case 0x271:                                  // [title]（+0x36d）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[title]", 0x271));
                    }
                    else
                    {
                        ScanStr(&script->m_field00);
                        break;
                    }
                }
                /* fall through */
                case 0x274:                                  // [explain]（+0x3da）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[explain]", 0x274));
                    }
                    else
                    {
                        ScanStr(&script->m_field04);
                        break;
                    }
                }
                /* fall through */
                case 0x277:                                  // [db table]（+0x44a）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[db table]", 0x277));
                    }
                    else
                    {
                        ScanStr(&script->m_dbTableName);
                        break;
                    }
                }
                /* fall through */
                case 0x27a:                                  // [fatigue]（+0x4ba）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[fatigue]", 0x27a));
                    }
                    else
                    {
                        script->m_field0c = ScanInt((bool*)0);
                        break;
                    }
                }
                /* fall through */
                case 0x299:                                  // [reward explain]（+0x52e）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[reward explain]", 0x299));
                    }
                    else
                    {
                        ScanStr(&script->m_str70);
                        break;
                    }
                }
                /* fall through */
                case 0x29c:                                  // [final reward explain]（+0x59e）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[final reward explain]", 0x29c));
                    }
                    else
                    {
                        ScanStr(&script->m_str74);
                        break;
                    }
                }
                /* fall through */
                case 0x29f:                                  // [reward]（+0x60e）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[reward]", 0x29f));
                    }
                    else
                    {
                        for (;;)
                        {
                            std::pair<int, int> attend;     // +0x65c 每轮默认构造
                            attend.first = ScanInt(&ok);    // +0x673
                            if (!ok)
                                break;                      // +0x68a → 公共尾
                            attend.second = ScanInt(&ok);   // +0x697
                            if (!ok)
                                break;
                            script->m_attends.push_back(attend);   // +0x6c5
                        }
                        break;
                    }
                }
                /* fall through */
                case 0x2ad:                                  // [final day condition]（+0x6d1）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[final day condition]", 0x2ad));
                    }
                    else
                    {
                        script->m_field24 = ScanInt((bool*)0);
                        break;
                    }
                }
                /* fall through */
                case 0x2b0:                                  // [final reward]（+0x736）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[final reward]", 0x2b0));
                    }
                    else
                    {
                        script->m_pair1c.first  = ScanInt((bool*)0);   // +0x1c
                        script->m_pair1c.second = ScanInt((bool*)0);   // +0x20
                        break;
                    }
                }
                /* fall through */
                case 0x2c7:                                  // [send mail]（+0x7ad）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[send mail]", 0x2c7));
                    }
                    else
                    {
                        importActionMailScript(&script->m_mail);
                        break;
                    }
                }
                /* fall through */
                case 0x2cb:                                  // [msg overbtn]（+0x80b）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[msg overbtn]", 0x2cb));
                    }
                    else
                    {
                        ScanStr(&script->m_str28);
                        break;
                    }
                }
                }

                if (!__inited)
                {
                    __init_lock.leave();                    // +0x87c
                    __inited = true;                        // +0x881
                }
                else
                {
                    break;
                }
                    continue;   // +0x888 → guard 重派发
        }
    }
scan_done:
    return ok;
