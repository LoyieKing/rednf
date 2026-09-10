int importPremiumServiceData(std::vector<PremiumServiceData>& out)
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    if (!loadRDARScriptFile("", "Etc/PremiumServiceEffect.etc"))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3044,
                              "CRITICAL ERROR : importPremiumServiceData load faild!");
        return 0;
    }
    char framePad[4];                             // [b28d-solo] P232 槽基探针
    std::string token;                            // ORIG -0xfc（先构造）
    PremiumServiceData data;                      // ORIG -0x150
    for (;;)
    {
        if (!ScanType(token, true))
            break;                                // ORIG 出口岛在函数尾（共享 leave）
        if (token == "[premium service]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                {
                scan_exit:
                    goto scan_done;
                }
            redo_lookup:
                static LookupMap __lookup;        // guard bss 0x094f71b8
                static SpinLock __init_lock;      // guard bss 0x094f71c0
                static bool __inited = false;     // bss 0x094f72d4
                int id;                           // ORIG -0x1c
                if (__inited)
                {
                    const char* key = token.c_str();     // ORIG -0xf4
                    LookupMap::const_iterator it = __lookup.find(key);
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
                case 0x3059:                       // [type]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[type]", 0x3059));
                        }
                    else
                    {
                        data.m_type = ScanInt((bool*)0);
                        break;
                    }
                case 0x305c:                       // [icon index]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[icon index]", 0x305c));
                        }
                    else
                    {
                        data.m_iconIndex = ScanInt((bool*)0);
                        break;
                    }
                case 0x305f:                       // [icon explain]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[icon explain]", 0x305f));
                        }
                    else
                    {
                        ScanStr(&data.m_iconExplain);
                        break;
                    }
                case 0x3062:                       // [add equipment list]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[add equipment list]", 0x3062));
                        }
                    else
                    {
                        for (;;)
                        {
                            bool ok = false;      // ORIG 每轮 movb $0
                            int v = ScanInt(&ok);
                            if (!ok)
                                break;
                            data.m_addEquipmentList.push_back(v);
                        }
                        break;
                    }
                case 0x306e:                       // [main premium image index]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[main premium image index]", 0x306e));
                        }
                    else
                    {
                        data.m_mainImageIndex.first = ScanInt((bool*)0);
                        data.m_mainImageIndex.second = ScanInt((bool*)0);
                        break;
                    }
                case 0x3072:                       // [additional premium image index]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[additional premium image index]", 0x3072));
                        }
                    else
                    {
                        data.m_additionalImageIndex = ScanInt((bool*)0);
                        break;
                    }
                case 0x3076:                       // [add selectAble equipment list]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[add selectAble equipment list]", 0x3076));
                        }
                    else
                    {
                        for (;;)
                        {
                            bool ok = false;
                            int a = ScanInt(&ok);
                            if (!ok)
                                break;
                            int b = ScanInt(&ok);   // 复用 ok，不重置（ORIG +0x6b8）
                            if (!ok)
                                break;
                            data.m_selectAbleEquipmentList.insert(
                                std::make_pair(a, std::make_pair(false, b)));
                        }
                        break;
                    }
                case 0x3088:                       // [/premium service]
                    if (!__inited)
                    {
                        __lookup.insert(LookupMap::value_type("[/premium service]", 0x3088));
                        }
                    else
                    {
                        out.push_back(data);
                        data.clear();
                        break;
                    }
                default:
                    break;
                }
                if (!__inited)                    // ORIG xor$1/test/je 回边形
                {
                    __init_lock.leave();
                    __inited = true;
                    goto redo_lookup;
                }
            }
        scan_done:
            ;
        }
    }
    return 1;
