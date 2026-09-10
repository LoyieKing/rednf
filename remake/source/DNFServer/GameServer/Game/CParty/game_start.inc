void CParty::game_start(CUser* user)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS vector<int> begin/end/de/pp/ne + GetPartyTelePort
    // 6 项 / EXTRA operator[]（P96 迭代器形，改型回归实测）。@0x859d718
    if (m_state == 2)
        return;

    PacketGuard basicPacket;                                // -0x54
    basicPacket.clear();
    basicPacket.put_header(0, 2);
    basicPacket.put_byte(1);
    basicPacket.put_short(get_member_count());

    m_tournamentVictory = 0;                                // +0x11a
    m_routingDice = get_rand_int(get_member_count());          // +0x144 路由起始槽
    m_field185c = GlobalData::s_systemTime_.getCurSec();    // +0x185c

    PacketGuard enterPacket;                                // -0x60
    enterPacket.put_header(0, 3);
    int liveCount = 0;                                      // -0x40
    int countIdx = enterPacket.get_index();                 // -0x64
    enterPacket.put_byte(liveCount);

    char hackFlag = 0;                                      // -0x3a
    char hostVill = user->getCurCharacVill();               // -0x39
    int hostArea = user->get_area(false);                   // -0x38

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        char vill = m_member[i].m_pUser->getCurCharacVill();   // -0x1d
        int area = m_member[i].m_pUser->get_area(false);       // -0x1c
        bool mismatch = ((hostVill != vill) || (hostArea != area));
        if (mismatch && !is_quick_party() &&
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                    ->m_villageMonsterId == 0)                  // ORIG this+0xc4c
        {
            LogManager::logFormat(
                1, "party.cpp", __PRETTY_FUNCTION__, 0x84c,
                "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
                NumberToString(m_member[i].m_pUser->get_acc_id(), 0),
                (int)hostVill, (int)vill, hostArea, area);
            hackFlag = 1;
            break;
        }
        m_battleData.m_arr54[i] = 1;      // +0x380
        m_battleData.m_arr58[i] = 1;  // +0x384
        m_battleData.m_arr5c[i] = 0;      // +0x388
        m_battleData.m_arr60[i] = 0;      // +0x38c
        m_member[i].m_pUser->make_basic_info((char*)&basicPacket, 1);
        G_GameWorld()->goto_dungeon(m_member[i].m_pUser);
        if (m_member[i].m_pUser->get_state() == 5)
        {
            enterPacket.put_short(
                m_member[i].m_pUser->get_unique_id() & 0xffff);
            enterPacket.put_byte(1);
            ++liveCount;
        }
        int tradeIdx = m_member[i].m_pUser->GetTradeSpace();   // -0x48
        if (tradeIdx != -1)
        {
            CTradeSpace* ts = G_CGameManager()->GetTradeSpace(tradeIdx);
            if (ts != 0 && ts->check_trade_possibility() != 1)
            {
                ts->cancel_trade();
                G_CGameManager()->PutTradeSpace(ts);
            }
        }
        m_member[i].m_pUser->getCurCharacQuestW()->m_authenData.reset();
    }

    if (hackFlag != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = get_user(i);
            if (u != 0)
            {
                LogManager::logFormat(
                    1, "party.cpp", __PRETTY_FUNCTION__, 0x88a,
                    "[HACK_VILL_AREA] m_id : %s, Vill(%d), Area(%d)",
                    NumberToString(u->get_acc_id(), 0), (int)hostVill,
                    hostArea);
                u->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
        }
        return;
    }

    enterPacket.put_byte(countIdx, liveCount);
    enterPacket.finalize(true);
    if (!IsExistInvisible())
    {
        if (getManager()->getMoveSpace() != 1)
            G_GameWorld()->send_all(enterPacket);
        else
            send_to_party(enterPacket);
    }
    else
    {
        send_to_party(enterPacket);
    }

    basicPacket.finalize(true);
    send_to_party(basicPacket);

    set_host();
    send_host_info();

    m_field1864 = 0;
    m_hellWorldMap = 0;
    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);   // -0x34
    if (worldIdx > 0)
        m_hellWorldMap =
            G_CDataManager()->m_worldMapList.find_world_map(worldIdx);

    basicPacket.clear();
    basicPacket.put_header(0, 0x1b);

    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
