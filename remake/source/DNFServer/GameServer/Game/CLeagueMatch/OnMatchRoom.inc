int CLeagueMatch::OnMatchRoom(PvP_Room* room, int authValue)
{
    if (m_authValue != authValue) {
        // [b28d-solo-CLeagueMatch] P221：ORIG 五处 trace 槽 -0x6c/-0x5c/-0x4c/
        // -0x3c/-0x2c（由深到浅），具名声明形 4.4 反向（-0x38→-0x78）⇒ 改
        // 「构造即调用」全表达式临时链翻槽方向。
        cMyTrace(__PRETTY_FUNCTION__, 0x295, 0)("OnMatchRoom timeout");
        return 0;
    }
    if (m_state != 1)
        return 0;

    // [b28d-solo-CLeagueMatch] P198③：ORIG 0x0855dca0/dca7/dcb4 三处预清
    // （Ghidra C：leader=0; theirRoom=0; opponent=0 在 Match 调用前）
    // ⇒ 函数头声明并置 0（双槽置 0 形），后文赋值；补 gap 2。
    CUser* leader = 0;
    PvP_Room* theirRoom = 0;
    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);   // ORIG 丢弃返回值，仅判 opponent==0
    if (opponent == 0) {
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    leader = opponent->GetUser(0);
    if (leader == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b0, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b0);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    theirRoom = reinterpret_cast<PvP_Room*>(leader->GetPVPRoom());
    if (theirRoom == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b9, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b9);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C `get_player_count(param_1)`
    // + ORIG 0x0855de5e `mov 0xc(%ebp),%eax` 实证）：此守卫检查的是**形参 room**
    // 的人数，非 theirRoom（theirRoom 仅用于下方 badSize）。
    if (room->get_player_count() > (int)nPlayerCountPerOneTeam_) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2c2, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2c2);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    bool badSize =
        (m_teamA.size() > (int)nPlayerCountPerOneTeam_) ||
        (theirRoom->get_player_count() > (int)nPlayerCountPerOneTeam_);
    // [SweepLeagueMatch4·L11 round1 补充实测] badSize 直接内联进 if（去具名 bool）
    // ⇒ ndiff 79→**111**、insn 277→276（劣化，已回滚）：ORIG 该处虽无槽，但 4.4 的
    // 具名 bool 槽恰与 ORIG 的匿名物化区重合 ⇒ 保留具名形为最优可接受形（N82/P332）。
    if (badSize) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2ca, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2ca);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    _BackupRoomInfo(theirRoom);
    m_teamB.reset();
    theirRoom->get_user_list(&m_teamB);
    _SendMatchStatePacket(3, 2);
    m_state = 3;
    // [b28d-solo-CLeagueMatch·S408 实测] ORIG 0x0855e009/e010 双预清（-0x14/-0x10）
    // 的 P191 双声明形（theirTeam/seat 前置 + for 再初始化）本轮实测 ndiff
    // 102→121（+2 insn，槽次数反劣）⇒ **回滚**，保持 for 内声明形。
    for (unsigned int seat = 0; seat <= 7; ++seat) {     // setbe 无符号 <=7
        CUser* member = reinterpret_cast<CUser*>(theirRoom->get_user_seat(static_cast<int>(seat)));
        if (member == 0)
            continue;
        int theirTeam = theirRoom->get_team(static_cast<int>(seat));
        _PullOutRoom(member, theirRoom, static_cast<int>(seat));
        int newSeat = _PutInRoom(member, room, false);
        if (theirTeam == 3)
            room->OnSetPVPSeatState(member, newSeat,
                                    static_cast<ENUM_SEAT_STATE>(3),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
    }
    pWaitingRoomList_->DelRoom(this);                   // IRoomList vtable+0x04
    m_state = 4;
    _SendMatchStatePacket(2, 2);
    return 1;
