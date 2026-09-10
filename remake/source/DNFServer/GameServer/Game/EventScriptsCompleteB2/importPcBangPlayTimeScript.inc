bool importPcBangPlayTimeScript(PcBangPlayTimeScript* script, char const* path)
{
    // b28d-solo-EventScriptsCompleteB2: IDENTICAL_AE — 复验 AE（原 b28d-L3；ORIG 0x08985601
    if (!loadRDARScriptFile("Event/", path))
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool result = false;          // ORIG -0x1d：仅 ScanType 失败路径读取返回

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
                case 0x5d2:                                  // [/event]（+0x2d3）
                case 0:
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/event]", 0x5d2));
                    }
                    else
                    {
                        return true;                        // +0x330 mov $1,%ebx
                    }
                }
                /* fall through */
                case 0x5d7:                                  // [title]（+0x33a）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[title]", 0x5d7));
                    }
                    else
                    {
                        ScanStr(&script->m_field00);
                        break;
                    }
                }
                /* fall through */
                case 0x5da:                                  // [explain]（+0x3a7）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[explain]", 0x5da));
                    }
                    else
                    {
                        ScanStr(&script->m_field04);
                        break;
                    }
                }
                /* fall through */
                case 0x5dd:                                  // [db table]（+0x417）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[db table]", 0x5dd));
                    }
                    else
                    {
                        ScanStr(&script->m_dbTableName);
                        break;
                    }
                }
                /* fall through */
                case 0x5e0:                                  // [play time]（+0x47b）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[play time]", 0x5e0));
                    }
                    else
                    {
                        // ORIG +0x4d5 值先物化经 %edx 再重载 this 存 +0xc。
                        // m_playTime int→unsigned 异型化（P211，InGameEventScripts.h
                        // 180 行）触发 4.4 转换中转，store 展开翻为 RHS 先 = ORIG 形。
                        script->m_playTime = ScanInt((bool*)0);
                        break;
                    }
                }
                /* fall through */
                case 0x5e3:                                  // [number of time]（+0x4e2）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[number of time]", 0x5e3));
                    }
                    else
                    {
                        script->m_numberOfTime =
                            static_cast<unsigned char>(ScanInt((bool*)0));
                        break;
                    }
                }
                /* fall through */
                case 0x5e6:                                  // [start tag]（+0x549）
                {
                    if (!__inited)
                    {
                        __lookup.insert(
                            LookupMap::value_type("[start tag]", 0x5e6));
                    }
                    else
                    {
                        script->m_startTag =
                            static_cast<unsigned short>(ScanInt((bool*)0));
                        break;
                    }
                }
                /* fall through */
                case 0x5e9:                                  // [end tag]（+0x5ae）
                {
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[end tag]", 0x5e9));
                    }
                    else
                    {
                        script->m_endTag =
                            static_cast<unsigned short>(ScanInt((bool*)0));
                        break;
                    }
                }
                }

                if (!__inited)
                {
                    __init_lock.leave();                    // +0x626
                    __inited = true;                        // +0x62b
                }
                else
                {
                    break;
                }
                    continue;   // +0x632 → guard 重派发
        }
    }
scan_done:
    return result;
