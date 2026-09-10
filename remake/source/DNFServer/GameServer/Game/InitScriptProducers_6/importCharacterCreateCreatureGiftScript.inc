bool importCharacterCreateCreatureGiftScript(CharacterCreateCreatureGiftScript* s,
                                             const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == 0)
    {
        return false;
    }

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    bool ok = false;                       // ORIG -0x10d
    int curJob = -1;                       // ORIG -0x114
    CharacterCreateCreatureGiftScript::eventJob eventJobItem;   // ORIG -0x11c

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        // ORIG 重派发跳回 +0x8e（__lookup guard）：静态声明位于循环体内。
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
            case 0x277:                      // [event job]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[event job]", 0x277));
                }
                else
                {
                    std::string jobName;    // ORIG -0x124 每轮构造/析构
                    ScanStr(&jobName);
                    curJob = getCharacterJobToEnumValue(jobName);
                    break;
                }
                /* fall through（ORIG +0x342 jmp +0x3a2） */
            case 0x27d:                      // [start]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[start]", 0x27d));
                }
                else
                {
                    ScanInt(&eventJobItem.m_minLevel);   // ORIG -0x11c
                    break;
                }
                /* fall through（+0x3fd jmp +0x412） */
            case 0x281:                      // [end]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[end]", 0x281));
                }
                else
                {
                    ScanInt(&eventJobItem.m_maxLevel);   // ORIG -0x11c+4
                    break;
                }
                /* fall through（+0x46d jmp +0x485） */
            case 0x285:                      // [/event job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event job]", 0x285));
                }
                else
                {
                    s->m_eventJobMap.insert(std::make_pair(curJob, eventJobItem));
                    eventJobItem.m_minLevel = -1;        // ORIG 复位三连
                    eventJobItem.m_maxLevel = -1;
                    curJob = -1;
                    break;
                }
                /* fall through（+0x4e0 jmp +0x567） */
            case 0x28c:                      // [change able creature lev]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[change able creature lev]", 0x28c));
                }
                else
                {
                    ScanInt(&s->m_changeAbleCreatureLev);  // ORIG +0x18
                    break;
                }
                /* fall through（+0x5b6 jmp +0x5cb） */
            case 0x290:                      // [change infinity creature]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[change infinity creature]", 0x290));
                }
                else
                {
                    while (true)
                    {
                        int first = 0;                   // ORIG -0x128
                        int second = 0;                  // ORIG -0x12c
                        first = ScanInt(&ok);
                        if (!ok) break;
                        second = ScanInt(&ok);
                        if (!ok) break;
                        s->m_clientChangeMap.insert(std::make_pair(first, second));
                    }
                    break;
                }
                /* fall through（+0x617 jmp +0x6d5） */
            case 0x2a2:                      // [client infinity creature]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[client infinity creature]", 0x2a2));
                }
                else
                {
                    while (true)
                    {
                        int second = 0;                  // ORIG -0x130（先声明）
                        int first = 0;                   // ORIG -0x134（后声明）
                        first = ScanInt(&ok);
                        if (!ok) break;
                        second = ScanInt(&ok);
                        if (!ok) break;
                        s->m_infinityChangeMap.insert(std::make_pair(first, second));
                    }
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_089607f4 公共尾：注册链走完释放锁并置位，重派发当前
            // 令牌；正常解析态直接 ScanType 下一轮。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return true;                           // ORIG EOF → ebx=1
