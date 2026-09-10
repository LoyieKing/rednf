bool importInGameEventInfoList(InGameEventInfoList* list, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    const char* key;                  // ORIG -0xb8（函数头声明，P309 先声明浅；无 init）
    std::string token;                // ORIG -0xc0
    bool result = false;              // ORIG -0x1d（0x0897eb97 movb $0）
    InGameEventInfo info;             // ORIG -0xd8（循环外 C1 0x0897eba4）

    while (true)
    {
        if (!ScanType(token, true))
        {
            break;                    // EOF → return result(false)（0x0897f0df）
        }

    redo_lookup:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            key = token.c_str();                     // 0x0897ecd6
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(key);
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                     // 0x0897ed8d
            if (__inited)
            {
                __init_lock.leave();                 // 0x0897eda4
                goto redo_lookup;                    // 0x0897edaa → guard
            }
            id = 0;                                  // 0x0897edaf
        }

        switch (id)
        {
        case 0:
        case 0x24:                       // [/event_list]（0x0897edce leaf）
            // ORIG 各 leaf `movzbl __inited; xor$1; test; je parse` —— 取反
            // 短路物化形（P32：!bool 判产生 xor$1），注册块落 if 体。
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[/event_list]", 0x24));
                break;                               // → 公共尾
            }
            return true;                             // 0x0897ee2b mov$1,%ebx
        case 0x27:                       // [idx]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[idx]", 0x27));
                break;
            }
            ScanInt(&info.m_eventId);                // 0x0897ee9b（返回值弃）
            ScanStr(&info.m_name);                   // 0x0897eeac
            break;
        case 0x2b:                       // [record type]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[record type]", 0x2b));
                break;
            }
            ScanInt(&info.m_field4);
            break;
        case 0x2e:                       // [start]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[start]", 0x2e));
                break;
            }
            ScanInt(&info.m_startTime);
            break;
        case 0x31:                       // [end]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[end]", 0x31));
                break;
            }
            ScanInt(&info.m_endTime);
            break;
        case 0x34:                       // [state]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[state]", 0x34));
                break;
            }
            ScanInt(&info.m_isEventing);             // +0x14 int（头注实证）
            break;
        case 0x37:                       // [/event]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[/event]", 0x37));
                break;
            }
            list->insertEventInfo(info.m_field4, info);   // 0x0897f0ae
            break;
        default:
            break;                                   // 未命中静默（含 "[event_list]" 开标签）
        }

        // ORIG 公共尾取反物化形：movzbl __inited; xor $1; test; je <nop 岛>
        // ⇒ 源码写负极性 `if (!__inited) { leave; 置位; 重派发; }`（P168a②）。
        if (!__inited)
        {
            __init_lock.leave();                     // 0x0897f0c8
            __inited = true;                         // 0x0897f0cd
            goto redo_lookup;                        // 0x0897f0d4 → guard 重派发
        }

    scan_next:;
    }
    return result;                                   // 共享退出岛 0x0897f0fe
