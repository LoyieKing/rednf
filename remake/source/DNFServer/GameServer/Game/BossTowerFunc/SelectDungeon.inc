bool Global::CBossTowerFunc::SelectDungeon(CParty* party, short dungeonIdx,
                                           ENUM_DUNGEON_TYPE type)
{
    if (party == 0)
    {
        return false;
    }

    // ORIG +0x1d/+0x21：两个具名字节局部（result 深于 memberCount）；
    // 决策树按 P163 补 case 1（与未列值同体）撑宽 range。
    // [P211/P300] handleStartGame 返回 char（ORIG `mov %al,-0x12` 直存，无
    // setne）；bool 目标会让 4.4 插 `test/setne` 归一化（多 2 条）⇒ 同址
    // union 双视角：写入走 char 视角（直存），读取走 bool 视角（xor 直测）。
    union ResByte
    {
        char c;
        bool b;
    };
    ResByte result;
    result.b = false;
    char memberCount = 0;

    WongWork::CBossTower* tower = G_CGameManager()->getBossTower();
    if (tower == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x34, 5);
        trace("StaticPool BossTower NULL");
    }
    else
    {
        const int member = party->get_member_count();
        switch (member)
        {
            case 1: memberCount = 0; break;
            case 2: memberCount = 1; break;
            case 3: memberCount = 2; break;
            case 4: memberCount = 3; break;
            default: break;  // ORIG: 越界值保持 0
        }

        result.c = tower->handleStartGame(party, static_cast<int>(dungeonIdx),
                                          memberCount, type);
        if (!result.b)
        {
            tower->_destroy();
        }
    }
    return result.b;
