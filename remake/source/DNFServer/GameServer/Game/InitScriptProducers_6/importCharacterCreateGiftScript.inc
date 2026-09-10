bool importCharacterCreateGiftScript(createChracScript* s, const char* path)
{
    // ORIG 双相派发（T 0x0895eda1）：函数级静态三件套位于主循环体内（ORIG
    // +0x8b/+0x107 每轮穿 guard），case 贯穿注册链按
    // [/create character gift]0x1ab→[apply job]0x1af→[item]0x1b5→
    // [new account event start time]0x1bc→[end time]0x1c2→[create title]0x1d2→
    // [create text]0x1d6→[/apply job]0x1da；执行形 0x1ab 段 = return true，
    // 时间转换 ret!=1 段 return false，EOF 出口 `movzbl -0x1d,%ebx` 读 ok 槽
    // （全函数无写入 ⇒ 恒 false）。
    // 收敛口径：orig=ours=637，助记符序列全等；残余仅 .o 未链接的 guard/静态
    // 对象/串字面量重定位显示（_ZZ…E8__lookup/11__init_lock/8__inited + 双
    // _ZGV guard 与 ORIG 同名，8 个 tag 串 .rodata 内容实测逐条一致），
    // 链接后按 AE 口径判等。
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                            // ORIG -0x108
    bool ok = false;                              // ORIG -0x1d（+0x4e 写，仅 EOF 出口读）
    createChracScript::rewardInfo info;           // ORIG -0x124
    int curJob = -1;                              // ORIG -0x128

    for (;;)
    {
        if (!ScanType(token, true))
            break;

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
            case 0x1ab:                      // [/create character gift]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[/create character gift]", 0x1ab));
                }
                else
                {
                    return true;              // ORIG +0x35c mov$1,%ebx
                }
                /* fall through（ORIG +0x35a jmp +0x366） */
            case 0x1af:                      // [apply job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[apply job]", 0x1af));
                }
                else
                {
                    std::string jobName;      // ORIG -0x130
                    ScanStr(&jobName);
                    curJob = getCharacterJobToEnumValue(jobName);
                    break;
                }
                /* fall through（+0x3c1 jmp +0x421） */
            case 0x1b5:                      // [item]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[item]", 0x1b5));
                }
                else
                {
                    int itemId = 0;           // ORIG -0x134（movl $0 先行）
                    ScanInt(&itemId);
                    info.m_items.push_back(itemId);
                    break;
                }
                /* fall through（+0x4ae jmp +0x4b3） */
            case 0x1bc:                      // [new account event start time]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[new account event start time]", 0x1bc));
                }
                else
                {
                    std::string str;          // ORIG -0x138
                    ScanStr(&str);
                    if (ConvertStringToTime(str, info.m_startTime) != 1)
                        return false;         // ORIG +0x597 mov$0,%ebx 出口
                    break;
                }
                /* fall through（+0x50e jmp +0x5e0） */
            case 0x1c2:                      // [new account event end time]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[new account event end time]", 0x1c2));
                }
                else
                {
                    std::string str;          // ORIG -0x13c
                    ScanStr(&str);
                    if (ConvertStringToTime(str, info.m_endTime) != 1)
                        return false;
                    break;
                }
                /* fall through（+0x63b jmp +0x70d） */
            case 0x1d2:                      // [create title]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create title]", 0x1d2));
                }
                else
                {
                    ScanStr(&info.m_title);   // ORIG info+0x14 直写
                    break;
                }
                /* fall through（+0x75c jmp +0x774） */
            case 0x1d6:                      // [create text]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create text]", 0x1d6));
                }
                else
                {
                    ScanStr(&info.m_text);    // ORIG info+0x18 直写
                    break;
                }
                /* fall through（+0x7c0 jmp +0x7d8） */
            case 0x1da:                      // [/apply job]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[/apply job]", 0x1da));
                }
                else
                {
                    if (curJob != -1)
                    {
                        std::map<int, createChracScript::rewardInfoList>::iterator it =
                            s->m_map0.find(curJob);
                        if (it == s->m_map0.end())
                        {
                            createChracScript::rewardInfoList list;
                            list.m_list.push_back(info);
                            s->m_map0.insert(std::make_pair(curJob, list));
                        }
                        else
                        {
                            it->second.m_list.push_back(info);
                        }
                    }
                    info.m_items.clear();     // ORIG +0x961..+0x9d8 复位六连
                    info.m_title.clear();
                    info.m_text.clear();
                    info.m_startTime = 0;
                    info.m_endTime = 0;
                    curJob = -1;
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_0895f783 公共尾。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return ok;                               // ORIG EOF 尾岛 movzbl -0x1d,%ebx
