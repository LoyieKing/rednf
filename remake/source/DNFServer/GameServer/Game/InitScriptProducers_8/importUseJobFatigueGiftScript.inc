bool importUseJobFatigueGiftScript(class useCharacterJobFatigue* out,
                                   const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;
    if (out == 0)
        return false;

    std::string token;
    bool ok = false;
    useCharacterJobFatigue::fatigueMatchGift gift;          // ctor 全清零
    int jobType = -1;                                       // ORIG local_128

    for (;;)
    {
        // ORIG 0x895f89f jne 0x895ffaf：EOF 走共享尾块返回 ok 槽（-0x11d）
        // 当前值（P164 配套：break + 循环外统一 return 得 ORIG 尾岛形）。
        if (!ScanType(token, true))
            break;

        // ORIG 0x895f8a5：三件套声明落循环体内（每轮穿 guard，P177）。
        static std::map<const char*, int, less_TCHAR> __lookup;  // ORIG __lookup
        static SpinLock __init_lock;                              // ORIG __init_lock
        static bool __inited = false;                             // ORIG __inited

    retry:
        // P333 状态机：首轮（未建表）v=0 进 case 0，沿 fallthrough 链逐条
        // 建表（ORIG 0x895fae7/0x895fb4e/0x895fc09/0x895fc7c/0x895fcef/
        // 0x895fe75），链尾置 __inited 后 goto retry 重查；此后 v=find 结果。
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator found =
                __lookup.find(token.c_str());
            v = (found != __lookup.end()) ? found->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            v = 0;                                           // 建表哨兵
        }

        // P315：ORIG 用 value_type 转发构造（pair<const char* const,int>
        // C1<char const(&)[N],int>），非 make_pair（后者多一层转换构造+EH）。
        typedef std::map<const char*, int, less_TCHAR>::value_type V;
        switch (v)
        {
        case 0:
        case 0x217:                                          // [/job fatigue]
            // ORIG 0x895faee：负极性 `xor $1; test; je`（P168a②：写 if(!__inited)）。
            if (!__inited)
                __lookup.insert(V("[/job fatigue]", 0x217));
            else
                return true;                                 // ORIG 0x895fb44
            // fallthrough
        case 0x21b:                                          // [job type]
            if (!__inited)
            {
                __lookup.insert(V("[job type]", 0x21b));
            }
            else
            {
                std::string jobName;
                ScanStr(&jobName);
                jobType = getCharacterJobToEnumValue(jobName);   // ORIG 0x895fbe0
                break;
            }
            // fallthrough
        case 0x229:                                          // [fatigue title]
            if (!__inited)
            {
                __lookup.insert(V("[fatigue title]", 0x229));
            }
            else
            {
                ScanStr(&gift.m_msg1);                       // +0x1c
                break;
            }
            // fallthrough
        case 0x22d:                                          // [fatigue text]
            if (!__inited)
            {
                __lookup.insert(V("[fatigue text]", 0x22d));
            }
            else
            {
                ScanStr(&gift.m_msg2);                       // +0x20
                break;
            }
            // fallthrough
        case 0x231:                                          // [reward list]
            if (!__inited)
            {
                __lookup.insert(V("[reward list]", 0x231));
            }
            else
            {
                for (;;)
                {
                    int first = 0;
                    int second = 0;
                    int third = 0;
                    first = ScanInt(&ok);
                    if (!ok)
                        break;
                    second = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (second < 0)
                        return false;                        // ORIG jns 校验
                    third = ScanInt(&ok);
                    if (!ok)
                        break;
                    if (third <= 0)
                        return false;                        // ORIG jg 校验
                    gift.m_field18 = third;                  // mov %eax,-0x140
                    gift.m_rewardMap.insert(std::make_pair(first, second));
                }
                break;
            }
            // fallthrough
        case 0x252:                                          // [/job type]
            if (!__inited)
            {
                __lookup.insert(V("[/job type]", 0x252));
            }
            else
            {
                out->m_giftMap.insert(
                    std::make_pair(jobType, gift));          // ORIG make_pairIRiRN..E
                gift.m_field18 = 0;                          // movl $0,-0x140
                gift.m_rewardMap.clear();                    // map::clear(-0x158)
                jobType = -1;                                // movl $-1,-0x124
                break;
            }
            break;
        default:
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }
    }
    return ok;
