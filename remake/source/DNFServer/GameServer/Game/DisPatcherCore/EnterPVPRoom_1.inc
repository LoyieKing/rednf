// [N8BigTURest 2026-09-11] 返回类型由 int 改 bool：ORIG 的
// _ZZ12EnterPVPRoom…E19__PRETTY_FUNCTION__ 实字节 = "bool EnterPVPRoom(CUser*,
// PvP_Room*, ENUM_CMDPACKET, const char*)"（.rodata 直证；mangled 不含返回类型）。
// DisPatchers.cpp:7051 的声明本就是 bool ⇒ 归一并补 __PRETTY_FUNCTION__ 符号（P139）。
// [N8BigTURest 2026-09-11 本轮读数] our 406→409（ORIG 409，gap 3→0）、ndiff 353→350，
//   TU 四数不变（215/666/2/49）；已落三处 ORIG 直证形：
//   ① `int cmdLog = cmd;`（ORIG 081ed4b8 `mov 0x10(%ebp),%eax; mov %eax,-0x60(%ebp)`，
//      日志第 6 变参从 -0x60 读）——原缺 2 条 store；
//   ② joinable 拆「=0 再赋值」（ORIG 081ed2e6 `movl $0x0,-0x24(%ebp)` 先于 085d6c44）；
//   ③ 分支布局改 `if (joinRes > 0) { 错误回包; return 0; }`（ORIG 081ed552
//      `cmpl $0x0,-0x20(%ebp); jle 成功路径` ⇒ 失败在前、成功 fallthrough）。
//   残余（判 N 封顶，勿再烧）：① ORIG 用 3 个 callee-saved（push %edi/%esi/%ebx）保活
//   roomIdx/characName/accName，4.4 只用 %esi/%ebx、将三者落栈 ⇒ 少 `push/pop %edi`；
//   ② 槽基常数差 8（我方 pg -0x38 vs ORIG -0x40；N126 版本首槽常数）；
//   ③ 帧 0x70 vs 0x7c。三者在 4.4 无源码形可择（N7/N126）。
bool EnterPVPRoom(CUser* user, PvP_Room* room, ENUM_CMDPACKET cmd,

                 char const* password)
{
    PacketGuard pg;                                     // -0x40(%ebp) local_44
    if (user->IsCurCharacGhost())                       // ORIG 080da2da
    {
        // put_header(1,cmd)/put_byte(0)/put_byte(0x13)/finalize/send
        pg.put_header(1, cmd);
        pg.put_byte(0);
        pg.put_byte(0x13);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }
    if (user->get_state() != 3)                         // ORIG 080da38c cmp $3
    {
        pg.put_header(1, cmd);
        pg.put_byte(0);
        pg.put_byte(0x13);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }
    if (user->getCurCharacVill() == 7)                  // ORIG 08645564 cmp $7,%al（PvP 区内）
    {
        user->SendCmdErrorPacket(cmd, 0x13, pg);        // ORIG 0867bfae
        return 0;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {                                                   // ORIG 085c4be8/085cad5e
        user->SendCmdErrorPacket(cmd, 0x3c, pg);
        return 0;
    }
    if (user->IsBusyDisjointStore())                    // ORIG 0864781e
    {
        user->SendCmdErrorPacket(cmd, 0x3c, pg);
        return 0;
    }
    if (room->get_pvp_battle_mode() == 6)               // ORIG 0822d9cc cmp $6
    {
        user->SendCmdErrorPacket(cmd, 0x17, pg);
        return 0;
    }
    if (user->CheckInTrade())                           // ORIG 080da2fe
    {
        user->SendCmdErrorPacket(cmd, 0x13, pg);
        return 0;
    }

    int joinable = 0;                                   // -0x24 local_28（ORIG 081ed2e6 先清 0）
    joinable = room->check_joinable(user);              // ORIG 085d6c44
    if (joinable != 0)
    {
        pg.put_header(1, cmd);
        pg.put_byte(0);
        pg.put_byte(joinable);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }

    if (room->IsExistPassword())                        // ORIG 0822d974
    {
        if (!room->CheckPassword(password,              // ORIG 085ddeae（xor $1 后 test）
                                 static_cast<int>(strlen(password))))
        {
            pg.put_header(1, cmd);
            pg.put_byte(0);
            pg.put_byte(6);                             // 密码错误码
            pg.finalize(true);
            user->Send(pg);
            return 0;
        }
    }

    if (user->CheckInParty())                           // ORIG 080da314
    {
        CParty* party = static_cast<CParty*>(user->GetParty());   // -0x1c（ORIG 0865514c）
        if (party != 0)
        {
            party->send_invite_pvp(user->get_unique_id() & 0xffff);  // ORIG movzwl 低 16 位
            G_CGameManager()->CheckOutParty(user, false);             // ORIG 080cc18e/08297c76
        }
    }

    int seat = 0;                                       // -0x44 local_48
    bool sameTeamJoin =
        G_GameWorld()->IsSameTeamChannelJoin();         // ORIG movzbl → bool 参
    int joinRes = room->join_room(user, seat, sameTeamJoin);   // -0x20 local_24（ORIG 085d6cf2）
    int cmdLog = cmd;                                   // -0x60（ORIG 081ed4b8 mov 0x10(%ebp)→-0x60）

    int battleMode = room->get_pvp_battle_mode();       // -0x5c（join_room 后再取，ORIG 0822d9cc）
    int roomIdx = room->get_index();                    // edi（ORIG 0822d994）
    char const* characName = user->getCurCharacName();  // esi（ORIG 08101028）
    char const* accName = user->get_acc_name();         // ebx（ORIG 08101072）

    CSwitchLog pvpLog(
        __PRETTY_FUNCTION__,
        0x6fa2, 0, 0);                                  // ORIG CSwitchLog::CSwitchLog 0854fd3c
    pvpLog("pvp@log %s,%s,room(%d),%d,%d,%d",
           accName, characName, roomIdx, battleMode, joinRes, cmdLog);

    if (joinRes > 0)                                    // ORIG 081ed552 cmpl $0,-0x20; jle 成功路径
    {
        pg.put_header(1, cmd);                          // joinRes > 0：加入失败错误码回包
        pg.put_byte(0);
        pg.put_byte(joinRes);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }

    {
        if (cmd == 0x65)                                // ORIG cmpl $0x65,0x10(%ebp)
        {
            PacketGuard pg65;                           // -0x50 local_54（带 try 域）
            pg65.put_header(1, 0x65);
            pg65.put_byte(1);
            pg65.finalize(true);
            user->Send(pg65);
        }
        room->make_seat_info(reinterpret_cast<char*>(&pg), seat);   // ORIG 085db9e2
        G_GameWorld()->send_all(pg);                    // ORIG 086c8c14
        G_GameWorld()->goto_pvp(user);                  // ORIG 086c6cee
        return 1;
    }
}
