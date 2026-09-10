bool importTitleSection(stTitleSection* section, int idx)
{
    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    std::string deadToken;          // ORIG -0xec（仅 ctor/dtor，P171 照抄）

    section->m_startIdx = idx * 10;                    // +0x0c
    section->m_endIdx = section->m_startIdx + 10;      // +0x10

    for (;;)
    {
        if (!ScanType(token, true))
            return true;

    dispatch:                       // ORIG +0x72：重派发当前令牌（不重扫）
        static LookupMap __lookup;      // ORIG _ZZ18importTitleSection..8__lookup
        static SpinLock __init_lock;     // ORIG ..11__init_lock
        static bool __inited = false;   // ORIG ..8__inited

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
                // 双检竞争失败方：解锁后重走查表（当前令牌，不重扫）。
                __init_lock.leave();
                goto dispatch;
            }
            id = 0;                     // 首遍：id=0 驱动级联注册链
        }

        switch (id)
        {
        case 0:
        case 0x155:                                    // [/section]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/section]", 0x155));
            else
                return false;
            // fall through（注册链）
        case 0x158:                                    // [/reward type]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[/reward type]", 0x158));
            else
                return true;
            // fall through
        case 0x15c:                                    // [section name]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[section name]", 0x15c));
            else
            {
                ScanStr(&section->m_sectionName);       // +0x04
                break;
            }
            // fall through
        case 0x15f:                                    // [section icon]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[section icon]", 0x15f));
            else
            {
                ScanStr(&section->m_icon);              // +0x24
                section->m_iconId = ScanInt((bool*)0);  // +0x28
                break;
            }
            // fall through
        case 0x166:                                    // [frame type]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[frame type]", 0x166));
            else
            {
                section->m_frameType = ScanInt((bool*)0);  // +0x2c
                break;
            }
            // fall through
        case 0x169:                                    // [lock message]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[lock message]", 0x169));
            else
            {
                ScanStr(&section->m_lockMessage);       // +0x30
                break;
            }
            // fall through
        case 0x16e:                                    // [name]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[name]", 0x16e));
            else
            {
                ScanStr(&section->m_name);              // +0x14
                break;
            }
            // fall through
        case 0x172:                                    // [image path]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[image path]", 0x172));
            else
            {
                ScanStr(&section->m_imagePath);         // +0x18
                section->m_imageParam = ScanInt((bool*)0);  // +0x1c
                break;
            }
            // fall through
        case 0x179:                                    // [effect path]
            if (!__inited)
                __lookup.insert(LookupMap::value_type("[effect path]", 0x179));
            else
            {
                ScanStr(&section->m_effectPath);        // +0x20
                break;
            }
            // fall through（注册链尾）
        default:
            break;
        }

        // ORIG +0x6a3 公共尾：注册链上 leave+置位并回循环头重派发；
        // __inited 后每个令牌（含标签）都会进入 importRewards。
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto dispatch;
        }

        importRewards(&section->m_rewards, token);
    }
