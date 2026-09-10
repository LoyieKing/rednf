bool importAvatarDisjointRandomRewardScript(AvatarDisjointRandomRewardScript* script,
                                            char const* path)
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
            case 0x32e:                                     // [/event]（+0x29d）
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/event]", 0x32e));
                }
                else
                {
                    return true;                            // +0x2fa
                }
            }
            /* fall through */
            case 0x330:                                     // [title]（+0x304）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title]", 0x330));
                }
                else
                {
                ScanStr(&script->m_field00);
                        break;
                }
            }
            /* fall through */
            case 0x333:                                     // [explain]（+0x365）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[explain]", 0x333));
                }
                else
                {
                ScanStr(&script->m_field04);
                        break;
                }
            }
            /* fall through */
            case 0x336:                                     // [random reward item]（+0x3c6）
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[random reward item]", 0x336));
                }
                else
                {
                for (;;)
    {
                        char         rewardKey = 0;         // -0xb5 每轮清 0
                        unsigned int cnt = 0;               // -0xbc
                        rewardKey = static_cast<char>(ScanInt(&ok));
                    if (!ok)
                            break;                          // +0x44a → 公共尾
                    cnt = static_cast<unsigned int>(ScanInt(&ok));
                    if (!ok)
                            break;                          // +0x46e → 公共尾
                        // 引用对临时(-0x38)+value_type 转换(-0x40)+insert(-0x48)
                        // 三段 sret 内联实参形（P134/P61）。
                    script->m_map08.insert(
                            std::make_pair(rewardKey, cnt));
                }
                    break;
                }
            }
            /* fall through */
            case 0x346:                                     // [send mail]（+0x4ca）
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[send mail]", 0x346));
                }
                else
                {
                    importActionMailScript(&script->m_mail);   // +0x20
                    break;
                }
            }
            }

            if (!__inited)
            {
                __init_lock.leave();                        // +0x53b
                __inited = true;                            // +0x540
            }
            else
            {
                break;
            }
                continue;   // +0x547 → guard 重派发
        }
    }
scan_done:
    return ok;
