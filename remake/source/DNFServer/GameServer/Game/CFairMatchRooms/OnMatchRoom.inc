int CFairMatch::OnMatchRoom(PvP_Room* room, int notifyId)
{
    if (m_matchNotifyId != notifyId) {
        cMyTrace(__PRETTY_FUNCTION__, 0xc8b, 0)("OnMatchRoom timeout");
        return 0;
    }
    if (m_field04 != 1)
        return 0;

    // P3：ORIG `call; cmp $0x1,%al; sete`（字节比较、无 movzbl）⇒ 实参收窄。
    room->setReamtchable(static_cast<unsigned char>(GetPlayCountPerOneTeam()) == 1);
    // P171：ORIG `movl $0x0,-0x2c(%ebp)`（+0x088）全函数只写不读，照抄死写入。
    int deadSlot = 0;
    (void)deadSlot;
    // ORIG 循环目标槽 -0x80 在函数入口清零一次，循环内不重置。
    PvP_Room* moveTarget = 0;
    m_leaderSeat[0] = room->GetManagerSeat();

    IMatch* other = 0;
    // P0/P168a：ORIG 无 bool 槽（`call; xor $0x1,%eax; test %al,%al; je`）⇒ 直测。
    if (!pWaitingRoomList_->Match(this, &other)) {   // 等待表 vtable[+0x08]=Match
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    PvP_Room* movable = 0;
    if (_CheckMovableRoom(other, room, &movable) == 0) {
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            // P171：ORIG nameBuf 为 255 字节（memset 0xff），单一缓冲复用两次。
            // P227：全表达式临时链 + GetUserName 返回值（ORIG %ebx 跨 ctor 保活）。
            char nameBuf[255] = {0};
            CSwitchLog("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                       0xcb0, 0, 0)("pvp@log 1 call match(%x), %s ",
                                   reinterpret_cast<void*>(this),
                                   GetUserName(this, nameBuf));
            CSwitchLog("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                       0xcb1, 0, 0)("pvp@log 1 opposite match(%x), %s ",
                                   other, GetUserName(other, nameBuf));
        }
        return 0;
    }

    if (room->get_index() == movable->get_index()) {
        // 撞进同一间：只作战斗开始兜底
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            char nameBuf[255] = {0};
            CSwitchLog("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                       0xcbb, 0, 0)("pvp@log 2 call match(%x), %s ",
                                   reinterpret_cast<void*>(this),
                                   GetUserName(this, nameBuf));
            CSwitchLog("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                       0xcbc, 0, 0)("pvp@log 2 opposite match(%x), %s ",
                                   other, GetUserName(other, nameBuf));
        }
        return 0;
    }

    // 正式配成：沿其它匹配单元的 next-match 链把对方队伍搬进来。
    // ORIG 迭代变量直接复用 other 槽（-0x84，GetNextMatch 结果写回同槽）。
    m_teams[1].reset();                       // TeamInfoThird::reset(+0x64)
    m_field04 = 3;
    int leaderTeamSide = 0;                   // ORIG -0x28
    CUser* managerUser = 0;                   // ORIG -0x24
    for (; other != 0; other = other->GetNextMatch()) {
        if (_CheckMovableRoom(other, room, &moveTarget) == 0) {
            m_field04 = 2;
            _SendMatchStatePacket(1, 1);
            return 0;
        }
        // ORIG receiver 是 this（`mov 0x8(%ebp),%eax; mov (%eax),%eax; add $0x84;
        // ... call *%edx`），非 other；GetPlayCountPerOneTeam 按 uchar 截断后比。
        if (GetPlayerCount()
            == static_cast<int>(static_cast<unsigned char>(
                   GetPlayCountPerOneTeam()))) {
            leaderTeamSide = 1;
            managerUser = reinterpret_cast<CUser*>(moveTarget->GetManager());
            _BackupRoomInfo(moveTarget);
        }
        // ORIG 访问 other->m_teams[0].m_players[p]（user@other+0x24+p*0x10、
        // flag@other+0x28+p*0x10），非 m_teams[1]；每次全量重算地址（P183）。
        for (int p = 0; p < 4; ++p) {
            if (static_cast<CFairMatch*>(other)->m_teams[0].m_players[p].m_flag != 0
                && static_cast<CFairMatch*>(other)->m_teams[0].m_players[p].m_user != 0)
                m_teams[leaderTeamSide].add(
                    static_cast<CFairMatch*>(other)->m_teams[0].m_players[p].m_user);
        }
        _SendMatchStatePacket(3, 2);
        // ORIG 压栈 0x4(%esp)=room(to)、0x8(%esp)=moveTarget(from)。
        _MoveRoom(room, moveTarget, leaderTeamSide == 0);
    }
    m_leaderSeat[1] = room->get_user_seat(managerUser);
    // P171：ORIG 直接解引用 pWaitingRoomList_（无 test/je null 守卫）。
    pWaitingRoomList_->DelRoom(this);
    m_field04 = 4;
    _SendMatchStatePacket(2, 2);
    m_fieldAC = 0;
    // P24/P0：ORIG gen_timer_key 结果直落 %esi 跨 get_index/G_TimerQueue 保活。
    G_TimerQueue()->InsertTimer(
        static_cast<TimerEntry::OBJ_TYPE>(5), room->get_index(),
        static_cast<TIMER_MESSAGE>(0x3b), 0xb4,
        static_cast<int>(room->gen_timer_key(static_cast<TIMER_MESSAGE>(0x3b))), 0);
    PacketGuard pkt;
    room->make_seat_info(reinterpret_cast<char*>(&pkt), 0xffff);
    G_GameWorld()->send_all(pkt);
    for (int t = 0; t < 2; ++t) {
        pkt.clear();
        pkt.put_header(0, 0x13d);
        pkt.put_short(m_leaderSeat[t]);
        pkt.finalize(true);
        m_teams[t].SendToTeam(pkt);
    }
    return 1;
