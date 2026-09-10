void CAssaultMgr::AvailableAssault(CUser* user, CUser const* source)
{
    // [b23S3C] P0/P183 槽映射：ORIG 仅 3 槽 -0x14(errorOut，lea 直发三处 out 参、
    // get_unique_id 复用)/-0x10(party)/-0xc(ageLimit=0xf)；删 minAgeOut/
    // partyAgeOut/partyLevelOut/ageOk/partyAgeOk/partyLevelOk/vill 缓存槽
    // （P48 调用点级直传+直判），bool 汇聚改嵌套 if。
    int errorOut = 0;                                   // -0x14
    CParty* party = 0;                                  // -0x10
    int ageLimitFlag = 0xf;                             // -0xc

    // [b23S3C] 短路 && 直跳形（嵌套 if 反产 setcc 物化）。
    if (PowerWarOn() && user->getCurCharacVill() == 7)
    {
        ageLimitFlag = 0xc;
    }

    if (user->CheckInParty() != 0)
    {
        party = static_cast<CParty*>(user->GetParty());
    }

    if (user->CheckLimitMiniumAge(ageLimitFlag, errorOut) == 0)
    {
        throw static_cast<ENUM_ERROR>(0x5f);
    }

    // 等级门槛：level>29 才放行，否则同源/异源两种错误码。
    if (user->get_charac_level() <= 0x1d)
    {
        errorOut = user->get_unique_id();
        if (user == source)
        {
            throw static_cast<ENUM_ERROR>(0xd);
        }
        throw static_cast<ENUM_ERROR>(0xe);
    }

    if (party != 0)
    {
        // [b28c-L7] P168a①：is_limit_minimum_age/level 已按 Main 授权 bool 化
        // （CParty.h:680 判词）⇒ 裸形直产 ORIG `call; xor $1,%eax; test %al,%al;
        // je`（探针 /tmp/xprobe.cpp b/j 形 = bool callee 唯一可翻形）。
        if (!party->is_limit_minimum_age(ageLimitFlag, errorOut))
        {
            throw static_cast<ENUM_ERROR>(0x5f);
        }
        if (!party->is_limit_minimum_level(0x1e, errorOut))
        {
            if (user == source)
            {
                throw static_cast<ENUM_ERROR>(0xfe);
            }
            throw static_cast<ENUM_ERROR>(0xff);
        }
    }
    // ORIG 函数自然收尾（leave/ret），无返回值语义。
