// ============================================================================
// importHeroMissionConditionsData —— ORIG T 0x896171b（无报告，nm+objdump 0x
// 逐指令转写）。RDA 脚本 "Event/HeroMissionEvent.evt" 解析：
//   [/hero event] [mission] [type] [repeat] [reset] [item] [condition]
//   [mail title] [mail content] [/mission]
// 返回：0=成功；非 0=出错脚本行号（loadScript 以 0 判活）。
// 首轮经 switch fallthrough 链惰性填充标签表（__lookup @0x94f7578 / SpinLock
// @0x94f7590 / 就绪字节 @0x94f7594，P21 范式），随后重查当前行分派。
// ============================================================================
// importHeroMissionConditionsData @ 0x0896171b —— [b28d-solo-HeroMissionEvent ·
//   N89/N82 封顶] 732 vs 717、gap 15、p167 CLEAR（0 missing/0 extra）。
//   剩余 ORIG-only 18 条 = movl 6（dead 局部初始化）+ jmp 8（决策树块排布）
//   + xor/jg/movw/add 各 1；槽基差 4（ORIG cond -0x16c ↔ OURS -0x170）属 N95；
//   4.1 对 vector/map 模板 out-of-line 实例（N89）+ 4.4 槽分配序（N82）不可翻。
//   配套：RewardData 用户空析构删除（P317）后 EXTRA +2 D1 消失。
int importHeroMissionConditionsData(std::vector<HeroMissionCondition>* out,
                                    const char* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x3e0;
    if (out == 0)
        return 0x3e3;

    std::string line;
    HeroMissionCondition cond;
    cond.clear();

    while (ScanType(line, true))
    {
    loop_top:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited;

        int code;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(line.c_str());
            code = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            code = 0;
        }

        switch (code)
        {
        case 0:
        case 0x3f0:   // [/hero event]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[/hero event]", 0x3f0));
            else
                return 0;
        case 0x3f4:   // [mission]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[mission]", 0x3f4));
            else
            {
                int t;
                if (!ScanInt(&t))
                    return 0x3f7;
                if (t <= 0 || t > 6)
                    return 0x3fc;
                cond.m_missionType = t;
            }
        case 0x400:   // [type]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[type]", 0x400));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x403;
                int t = ConvertString2EnumMissionType(s);
                if (t == 0)
                    return 0x407;
                cond.m_field04 = t;
            }
        case 0x40c:   // [repeat]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[repeat]", 0x40c));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x40f;
                cond.m_isRepeatMission = (s == "true");
            }
        case 0x414:   // [reset]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[reset]", 0x414));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x417;
                cond.m_isResetState = (s == "true");
            }
        case 0x41c:   // [item]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[item]", 0x41c));
            else
            {
                int cnt;
                if (!ScanInt(&cnt))
                    return 0x421;
                for (int i = 0; i < cnt; ++i)
                {
                    HeroMissionCondition::RewardData reward;
                    if (!ScanInt(&reward.m_itemIdx))
                        return 0x428;
                    int add;
                    if (!ScanInt(&add))
                        return 0x42b;
                    reward.m_addInfo = static_cast<unsigned short>(add);
                    cond.m_rewards.push_back(reward);
                }
            }
        case 0x431:   // [condition]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[condition]", 0x431));
            else
            {
                int cnt;
                if (!ScanInt(&cnt))
                    return 0x436;
                for (int i = 0; i < cnt; ++i)
                {
                    int v;
                    if (!ScanInt(&v))
                        return 0x43c;
                    cond.m_extra.push_back(static_cast<unsigned short>(v));
                }
            }
        case 0x442:   // [mail title]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[mail title]", 0x442));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x445;
                cond.m_mailTitle = s;
            }
        case 0x44a:   // [mail content]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[mail content]", 0x44a));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x44d;
                cond.m_mailContent = s;
            }
        case 0x452:   // [/mission]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[/mission]", 0x452));
            else
            {
                for (std::vector<HeroMissionCondition>::iterator it = out->begin();
                     it != out->end(); ++it)
                {
                    if (it->m_missionType == cond.m_missionType)
                        return 0x456;
                }
                out->push_back(cond);
                cond.clear();
            }
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto loop_top;
        }
    }
    return 0;
}