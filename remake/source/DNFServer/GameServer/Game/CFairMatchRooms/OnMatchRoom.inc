// OnMatchRoom ORIG T 0x8564150 报告 OnMatchRoom.md。ORIG 失败路径 → 0；成功 → 1。
// b28d-BigFairMatch 终判：NEAR——orig 500 / ours 500（gap 0），13 个差异对全部只是栈槽号
// （movable ORIG -0x94↔我方 -0x88、PacketGuard ORIG -0x90↔我方 -0x94，其余 15 个槽
// 全等；P167 调用点差集仅 C1/C2、D1/D2 同址别名 = P93 豁免）。残差属 N82 族：4.4 对
// 具名聚合对象优先占低槽（PacketGuard 落 -0x94），4.1 按生命周期穿插（movable 占 -0x94）；
// 声明前置/嵌套块/分离赋值三形实测均劣化（分别 13→28/36/…）⇒ 封 N82。
// b28d-BigFairMatch 本轮逐段核对 ORIG 汇编 + Ghidra C，修 3 处真语义 bug + 12 处形态缺口：
//   ① 循环内 GetPlayerCount() receiver 是 this（ORIG `mov 0x8(%ebp),%eax; ... call *%edx`），
//      原写 static_cast<CFairMatch*>(it)->GetPlayerCount()（对 other 调用）——真 bug；
//   ② 循环内读 other->m_teams[0].m_players[p]（ORIG other+0x24+p*0x10 user /
//      +0x28 flag），原写 m_teams[1]（+0x6c）——真 bug（错队伍索引）；
//   ③ P3：`cmp $0x1,%al`（无 movzbl）⇒ setReamtchable 实参 uchar 收窄；
//   ④ P0/P168a：删具名 bool matched（ORIG `call; xor $1,%eax; test %al,%al; je` 无槽）；
//   ⑤ P171：ORIG 入口 `movl $0x0,-0x2c(%ebp)` 死写入照抄（全函数无读）；
//   ⑥ ORIG moveTarget(-0x80) 函数入口清零一次，循环内不重置；循环迭代变量直接复用
//      other 槽（-0x84，GetNextMatch 结果写回），无独立 it 槽；
//   ⑦ P171：nameBuf 255 字节（ORIG `mov $0xff,%ebx` + 4.1 对齐慢路），非 256；
//   ⑧ P171：DelRoom 无 pWaitingRoomList_ null 守卫（ORIG 直解引用）；
//   ⑨ P24/P0：删具名 timerKey（ORIG gen_timer_key 结果直落 %esi 跨 get_index/G_TimerQueue）；
//   ⑩ P183：删 `char* gb` 与 `PlayerInfoThird& s` 缓存（ORIG 每点重算地址）；
//   ⑪ 真 bug：_MoveRoom 实参方向反——ORIG 压栈 0x4(%esp)=room、0x8(%esp)=moveTarget
//      （Ghidra C `_MoveRoom(this,param_1,local_84,…)`）⇒ `_MoveRoom(room, moveTarget, …)`；
//   ⑫ P227/P87：CSwitchLog 改全表达式临时链 + 用 GetUserName 返回值（ORIG `mov %eax,%ebx`
//      跨 ctor 保活后 `mov %ebx,0xc(%esp)`，%s 实参是返回值非数组名）。
// [GW20Run 2026-09-11 复核] fresh 500/500（NEAR）；p167 50=50 仅 CSwitchLog C1/C2、
// PacketGuard C1/C2·D1/D2、cMyTrace C1/C2 同址别名 ⇒ 无语义缺口，余槽组差封顶。
bool CFairMatch::OnMatchRoom(PvP_Room* room, int notifyId)
{
    if (m_matchNotifyId != notifyId) {
        cMyTrace(__PRETTY_FUNCTION__, 0xc8b, 0)("OnMatchRoom timeout");
        return false;
    }
    if (m_field04 != 1)
        return false;

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
        return false;
    }

    // [b28f-SweepCFairMatch 负结论] P212 声明提前（裸声明头置 + 原位赋值）
    // 实测反劣：ndiff 169→241（movable 仍 -0x88、pkt 仍 -0x94，槽未回正）
    // ⇒ 4.4 槽分区（聚合对象优先低槽）不可由标量声明序翻转，维持原形。
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
        return false;
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
        return false;
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
            return false;
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
    return true;
}