int DisPatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    // ORIG +0x20：errNo store 双钉（技45）。
    // [b28d-solo-DisPatcherCore] P211：ORIG RHS-first+edx 中转（unsigned 返回 + 无具名局部）。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1e5b, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG +0x84 `movb $0x0,-0x15` dead bool 初始化（技8 变体）：hasDeathTower
    // 结果落 bool 槽；worldIdx 槽 -0x14。
    bool deathTower = false;
    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
    if (worldIdx > 0)
    {
        CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        if (worldMap != 0)
        {
            deathTower = worldMap->hasDeathTower();   // ORIG +0xd3 `mov %al,-0x15`
        }
    }
    (void)deathTower;

    // ORIG +0xe8 `test %al,%al; je +0x134`：inParty 体是 fall-through 块（技17/19），
    // `get_state()==3` 为 cmp+sete+test 物化（技9 直写 ==3 已实证产 sete）。
    if (user->CheckInParty())
    {
        if (user->get_state() == 3)
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->game_start(user);
            }
        }
        return 0;
    }
    {
        CParty* party = G_CGameManager()->GetParty();
        if (party == 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x1e99,
                                  "cant party memory : you must free party memory!!!");
            return 0;
        }
        if (excludeAddHackCount(user))
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1f6, 1u, 0u, 0u);
        }
        party->set_single_play(user);
        party->game_start(user);
    }
    return 0;
