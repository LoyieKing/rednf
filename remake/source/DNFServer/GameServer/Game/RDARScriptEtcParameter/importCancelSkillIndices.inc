int importCancelSkillIndices(
    const char* path,
    std::map<std::pair<ENUM_CHARACTERJOB, int>, std::set<int> >& out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3372, "%s", path);
        return 0;
    }
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    char framePad[4];                           // [b28d-solo] P232 槽基探针
    std::pair<ENUM_CHARACTERJOB, int> key;      // ORIG -0xb4 C2Ev + 直写两成员
    key.first = ENUM_CHARACTERJOB(0xb);         // -0xb4 movl $0xb
    key.second = -1;                            // -0xb0 movl $-1
    std::set<int> skills;                       // ORIG -0xcc
    std::string token;                          // ORIG -0xd0
    for (;;)
    {
        if (!ScanType(token, true))
            break;
    redo_lookup:
        static LookupMap __lookup;              // guard bss 0x094f71d8
        static SpinLock __init_lock;            // guard bss 0x094f71e0
        static bool __inited = false;           // bss 0x094f7294
        int id;                                 // ORIG -0x1c
        if (__inited)
        {
            const char* tk = token.c_str();     // ORIG -0xa8
            LookupMap::const_iterator it = __lookup.find(tk);
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto redo_lookup;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0x3384:                            // [cancel skill]（无 parse）
            if (!__inited)
                            {
                                __lookup.insert(LookupMap::value_type("[cancel skill]", 0x3384));
                            }
                            else
                                break;
        case 0x3388:                            // [/cancel skill]（无 parse）
            if (!__inited)
                            {
                                __lookup.insert(LookupMap::value_type("[/cancel skill]", 0x3388));
                            }
                            else
                                break;
        case 0x338c:                            // [character job]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[character job]", 0x338c));
            }
            else
            {
                std::string jobName;            // ORIG -0xd8 块级
                if (!ScanStr(&jobName))
                    return 0;
                key.first = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
                std::string growName;           // ORIG -0xdc
                if (!ScanStr(&growName))
                    return 0;
                key.second = getGrowTypeIndex(growName.c_str());
                bool ok;                        // ORIG -0xdd 无初始化
                for (;;)
                {
                    int v = ScanInt(&ok);       // ORIG -0xe4
                    if (!ok)
                    {
                        out.insert(std::map<std::pair<ENUM_CHARACTERJOB, int>,
                                           std::set<int> >::value_type(key, skills));
                        key.first = ENUM_CHARACTERJOB(0xb);
                        key.second = -1;
                        skills.clear();
                        break;
                    }
                    skills.insert(v);
                }
                break;
            }
        case 0x33aa:                            // [/character job]（无 parse）
            if (!__inited)
                            {
                                __lookup.insert(LookupMap::value_type("[/character job]", 0x33aa));
                            }
                            else
                                break;
        default:
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto redo_lookup;
        }
    }
    return 1;
