void importSkillDataUp(std::vector<EquipmentSkillDataUp>& out)
{
    // [b28d-solo-EquipmentParameterInfo] P165/P177 结构还原（照 ORIG 0x088c049c 逐条
    // 转写）：对象构造序 e(-0xd4)→line(-0xd8)→reserve；`while (ScanStr(&line))`
    // 底部条件形；循环体内 static 三件套每轮穿 guard + 双检 goto guard_retry +
    // id=0 贯穿建表（case 0 与 0xa47 共享块，各 case insert 后链式 goto 下一 case）。
    EquipmentSkillDataUp e;
    std::string line;
    out.reserve(0x14);
    while (ScanStr(&line))
    {
        e.m_job = getCharacterJob(line.c_str());
        e.m_v4 = ScanInt((bool*)0);
        e.m_module = scanModuleType();
        ScanStr(&line);   // 标签 token
    guard_retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int id;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(line.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto guard_retry;
            }
            id = 0;
        }
        switch (id)
        {
        case 0:
        case 0xa47:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[static]", 0xa47));
                goto L_a4b;
            }
            e.m_key = 0; e.m_v8 = ScanInt((bool*)0);
            goto after_switch;
    L_a4b:
        case 0xa4b:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[level]", 0xa4b));
                goto L_a4f;
            }
            e.m_key = 1; e.m_v8 = ScanInt((bool*)0);
            goto after_switch;
    L_a4f:
        case 0xa4f:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[cooltime]", 0xa4f));
                goto L_a54;
            }
            e.m_key = 2; e.m_v8 = ScanInt((bool*)0); e.m_v8 = 0;   // ORIG 死写后清零（P171）
            goto after_switch;
    L_a54:
        case 0xa54:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[mp]", 0xa54));
                goto L_a59;
            }
            e.m_key = 3; e.m_v8 = ScanInt((bool*)0); e.m_v8 = 0;
            goto after_switch;
    L_a59:
        case 0xa59:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[maintain mp]", 0xa59));
                goto L_a5e;
            }
            e.m_key = 4; e.m_v8 = ScanInt((bool*)0); e.m_v8 = 0;
            goto after_switch;
    L_a5e:
        case 0xa5e:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[skill consume item]", 0xa5e));
                goto L_a6d;
            }
            e.m_key = 5; e.m_v8 = ScanInt((bool*)0);
            goto after_switch;
    L_a6d:
        case 0xa6d:
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("[casting time]", 0xa6d));
                goto L_default;
            }
            e.m_key = 6; e.m_v8 = ScanInt((bool*)0); e.m_v8 = 0;
            goto after_switch;
    L_default:
        default:
            break;
        }
    after_switch:
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto guard_retry;
        }
        ScanStr(&line);
        e.m_percent = (line == "+") ? 0 : 1;
        e.m_v10 = ScanInt((bool*)0);
        out.push_back(e);
    }
