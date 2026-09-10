bool ImportChattingEmoticon(EmoticonType::EmoticonScript& script,
                            const char* path)
{
    if (!loadRDARScriptFile(g_ChattingEmoticonScriptBaseDirectory, path))
        return false;                                   // +0x23 ebx=0
    script.m_index =
        g_ChattingEmoticonScriptFileList.FindIndexByFullScan(path);  // +0x34

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;      // ORIG -0x70（+0x45 构造）
    std::string value;      // ORIG -0x74（+0x50 构造）
    int dead20 = 0;         // ORIG -0x20 死写入，照抄

    for (;;)
    {
        if (!ScanType(token, true))    // +0x62（xor$1 物化）
        {
        scan_exit:                     // [P164] 回边落点：入口直落、出口共享
            goto scan_done;            // +0x45c：mov $1,%ebx → dtor 岛 → true
        }

    redo_lookup:                       // +0x80：两条重派发边（+0x23c/+0x451）落点
        static LookupMap __lookup;     // guard + __cxa_atexit(map::~map)
        static SpinLock __init_lock;   // guard + __cxa_atexit(SpinLock::~SpinLock)
        static bool __inited = false;  // 零初始化 plain bool（无 guard）

        int id;                        // ORIG -0x1c
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(token.c_str());  // -0x68 临时
            id = (it != __lookup.end()) ? it->second : -1;             // -0x60/-0x64
        }
        else
        {
            __init_lock.enter();                                       // +0x21f
            if (__inited)
            {
                __init_lock.leave();                                   // +0x236
                goto redo_lookup;                                      // +0x23c
            }
            id = 0;                                // +0x241 注册轮入口
        }

        switch (id)
        {
        case 0:
        case 0x2f:                      // [type]（+0x275 与 case 0 共 leaf）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[type]", 0x2f));
            }
            else
            {
                if (!ScanStr(&value))              // +0x2cc
                    return false;
                if (value == "EVENT")              // +0x2f0
                    script.m_type = 1;
                else if (value == "CASH")          // +0x313
                    script.m_type = 2;
                else
                    script.m_type = 4;
                if (!ScanInt(&script.m_param))     // +0x33b
                    return false;
                break;                             // → 公共尾 +0x430
            }
            /* fall through（+0x2c1 jmp +0x355） */
        case 0x40:                      // [ani]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[ani]", 0x40));
            }
            else
            {
                if (!ScanStr(&script.m_field0c))   // +0x3ac
                    return false;
                break;                             // → 公共尾
            }
            /* fall through（+0x39e jmp +0x3c2） */
        case 0x46:                      // [macro]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[macro]", 0x46));
            }
            else
            {
                if (!ScanStr(&script.m_field10))   // +0x419
                    return false;
                break;                             // → 公共尾
            }
        }

        if (!__inited)                 // 公共尾 +0x430（xor$1 物化）：
        {                              // inited → je 落尾部回跳岛（ORIG +0x456
            __init_lock.leave();                   //   nop;jmp +0x62，nop 剥离
            __inited = true;                       // +0x44e   后为 je;jmp 形）
            goto redo_lookup;                      // +0x451 → guard 重派发
        }
        goto scan_exit;                // [P164] 公共尾正常回边（ORIG +0x456）
    }
scan_done:
    return true;                                   // +0x4ac ebx=1
