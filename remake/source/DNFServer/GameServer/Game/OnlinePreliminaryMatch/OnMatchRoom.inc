int COnlinePreliminary::OnMatchRoom(PvP_Room* room, int authNo)
{
    if (m_authValue != authNo)
    {
        /* [b28d-solo-OnlinePreliminaryMatch·P221] ORIG 5 个 cMyTrace 槽
         * -0x6c/-0x5c/-0x4c/-0x3c/-0x2c（深→浅），4.4 对具名对象恒反向
         * （-0x38/-0x48/...）；改全表达式临时链复现 ORIG 槽深（P221 实证）。
         * 第 1 参用 __PRETTY_FUNCTION__（P139：ORIG 符号
         * _ZZ...OnMatchRoom...E19__PRETTY_FUNCTION__，手写串不生成该局部符号）。 */
        cMyTrace(__PRETTY_FUNCTION__, 0x7a6, 0)("OnMatchRoom timeout");
        return 0;
    }
    if (m_field04 != 1)
        return 0;

    /* [b28d-solo-OnlinePreliminaryMatch·P212 全头部声明] ORIG @08561555-0x79
     * 三条初始化 `movl $0,-0x1c/-0x18/-0x70`（oppUser/oppRoom/opponent 均在
     * 函数头声明 =0），使用点仅赋值 ⇒ 局部提至函数头。 */
    CUser* oppUser = 0;
    PvP_Room* oppRoom = 0;
    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);            /* vtable+8 虚调 */
    if (opponent == 0)
    {
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    oppUser = opponent->GetUser(0);                       /* vt+0x10 */
    if (oppUser == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7c1, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7c1);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    oppRoom = (PvP_Room*)oppUser->GetPVPRoom();                      /* T 0x865518e */
    if (oppRoom == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7ca, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7ca);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG `movzbl %al,%eax` 形 ⇒
     * `(unsigned char)` 强转（`& 0xff` 恒产 and $0xff）。
     * [b28d-solo-OnlinePreliminaryMatch 真语义 bug 修] ORIG @085616e5
     * `mov 0xc(%ebp),%eax` = 形参 room（非 local_1c/oppRoom；Ghidra C 同证
     * `get_player_count(param_1)`）——原写 oppRoom 查错对象。 */
    if (room->get_player_count() > (int)(unsigned char)GetPlayCountPerOneTeam())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7d2, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7d2);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG `movzbl %al,%eax` 形 ⇒
     * `(unsigned char)` 强转（`& 0xff` 恒产 and $0xff）。 */
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561790 `cmp %eax,%ebx;
     * jg`（ebx=size 在左）⇒ 比较方向必须写 size > pc（写成 pc < size 产 jl）。 */
    if (m_teams[0].size() > (int)(unsigned char)GetPlayCountPerOneTeam() ||
        oppRoom->get_player_count() > (int)(unsigned char)GetPlayCountPerOneTeam())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7da, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7da);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    _BackupRoomInfo(oppRoom);
    m_teams[1].reset();
    /* ORIG 经对方房间 get_user_list（其 match->GetUserList）填充 team1 视图：
     * 对方同为 COnlinePreliminary 时即 memcpy 其 teams[0]，布局天然兼容。*/
    oppRoom->get_user_list(&m_teams[1]);

    _SendMatchStatePacket(3, 2);
    m_field04 = 3;

    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @0856188c-0x93 循环前双初始化
     * `movl $0,-0x14`（team）+ `movl $0,-0x10`（s）⇒ team 是循环外声明
     * （P212），循环内仅赋值。 */
    int team = 0;
    for (unsigned int s = 0; s <= 7; ++s)
    {
        /* [b28d-solo-OnlinePreliminaryMatch] ORIG @085618b1 `mov %eax,
         * -0x1c(%ebp)`：循环内 user 复用 oppUser 槽（ORIG 全函数仅 5 个浅槽
         * -0xc/-0x10/-0x14/-0x18/-0x1c 承载 6 个局部）⇒ 原码就是复用 oppUser
         * 变量本身（oppUser 在取到 oppRoom 后已死）。另起 `CUser* u` 会多占
         * -0x10 槽使全帧槽号整体深 4（帧同为 0x90 但槽基 -4）。 */
        oppUser = reinterpret_cast<CUser*>(oppRoom->get_user_seat((int)s));
        if (oppUser == 0)
            continue;
        team = oppRoom->get_team((int)s);
        _PullOutRoom(oppUser, oppRoom, (int)s);
        /* [b28d-solo-OnlinePreliminaryMatch·P218/P290] ORIG @0856191d-0x41
         * 压参序为「先压常量 2/3 再装载 newSeat」；unsigned 局部使 4.4 提前
         * 装载 %eax。声明改 int 复现 ORIG 装载时刻。 */
        int newSeat = _PutInRoom(oppUser, room, false);
        if (team == 3)
            room->OnSetPVPSeatState(oppUser, newSeat,
                                    (ENUM_SEAT_STATE)3, (ENUM_WALKOUT_TYPE)2);
    }

    pWaitingRoomList_->DelRoom(this);
    m_field04 = 4;
    _SendMatchStatePacket(2, 2);
    m_p2pReadyCount = 0;

    /* [b28d-solo-OnlinePreliminaryMatch·P24] ORIG @085619b6-0x510 无 key 具名
     * 槽：gen_timer_key 结果直落 %esi 跨 get_index/G_TimerQueue call 保活 ⇒
     * 实参内联（具名局部会多 -0x14 槽往返 2 条）。 */
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)5, room->get_index(),
                                (TIMER_MESSAGE)0x3b, 0xb4,
                                (int)room->gen_timer_key((TIMER_MESSAGE)0x3b), 0);
    return 1;
