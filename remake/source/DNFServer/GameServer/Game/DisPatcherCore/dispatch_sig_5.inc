int DisPatcher_ResPeer::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // [H5DisPatcher 核账 2026-09-11] 实测 ours 2033→2023 ✅（9 处 bool/char/short 具名量内联）
    if (user->get_state() != 3)
    {
        return 0;
    }

    PacketGuard pgRes;                       // ORIG -0xac（全程复用）
    unsigned short peerIdx = 0;              // ORIG msg+0xd（local_97）
    char requestType = 0;                    // ORIG msg+0xf（local_95）
    unsigned int cookie = 0;                 // ORIG msg+0x10（local_94）
    if (!packet.get_short(peerIdx))
    {
        return LineFunc(0x77d5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(requestType))
    {
        return LineFunc(0x77d6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(cookie))
    {
        return LineFunc(0x77d7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // [b28d] P17/P261：ORIG 0x081f09b3 起为 switch 二分决策树
    // （cmp $6/je → 09f0、cmp $6/jg → 09c4、cmp $5/je → 09d0、cmp $7/je → 09d0、
    // cmp $8/je → 09f0、jmp default 0a10），非 if-else 链。
    // [G5-SigA] P0：ORIG 0x081f09xx 为 `call GetReceivedRequestType; movsbl %al,%eax;
    // cmp $0x6,%eax`（**无具名槽**，直接 32 位提升后比较）。原具名
    // `unsigned char recvType` 多出 `mov %al,-0x99(%ebp); movzbl -0x99(%ebp),%eax`
    // 2 条，且该 1B 槽使帧 0xec→0xf0（fn_diff_tu 实测 sub $0xf0 vs ORIG $0xec）。
    switch ((char)user->GetReceivedRequestType())
    {
    case 5:
    case 7:
        if (requestType != 5 && requestType != 7)
            return 0;
        break;
    case 6:
    case 8:
        if (requestType != 6 && requestType != 8)
            return 0;
        break;
    default:
        if ((char)user->GetReceivedRequestType() != requestType)
            return 0;
        break;
    }
    user->InitReceivedRequestType();

    switch (requestType)
    {
    case 0:                                  // ---- 组队邀请应答 ----
    {
        PacketGuard pgParty;                 // ORIG -0xb8
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte(3);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        // [DPC_SigA] P0/P183：ORIG 全函数**无 `mov %al,-0xNN(%ebp)` 形态的 bool 槽**
        // （multidiff：ORIG 独有 0 条 vs 我方 8 条）⇒ 各 bool 均为内联表达式。
        if ((target == user) ||
            (target->getCurCharacNo() ==
             user->getCurCharacNo()))
        {
            return 0x7828;
        }
        if (user->get_state() == 5 || target->get_state() == 5)
        {
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte((char)tpErr);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        if (target->get_state() != 3)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte(0x13);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        if (user->CheckInParty() && target->CheckInParty())
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte(0x12);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        if (user->isCompetitionMercenary())
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0xea));
            break;
        }
        if (target->isCompetitionMercenary())
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0xea));
            break;
        }

        if (GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            // ORIG 6 次 `_ZNK getCurCharacVill` 内联（无缓存局部）；两个 0x13
            // 错误包各带一个 clear(local_bc)。语义 = （双 7 且 power 侧异）∨
            // （恰一 7）→ 拒；否则落到 LAB_081f12d8 组队逻辑。
            // [DPC_SigA] P0/P183：selfVill/bothSeven 两个具名量（1B+1B）在 ORIG
            // 0x081f1xxx 无对应槽（6 次 getCurCharacVill 全内联）⇒ 直接判定。
            if (user->getCurCharacVill() == 7 &&
                target->getCurCharacVill() == 7)
            {
                if (user->getPowerSide() != target->getPowerSide())
                {
                    pgParty.clear();
                    pgParty.put_header(1, 0xb);
                    pgParty.put_byte(0);
                    pgParty.put_byte(0x13);
                    pgParty.put_byte(0);
                    pgParty.finalize(true);
                    user->Send(pgParty);
                    break;
                }
            }
            else if ((user->getCurCharacVill() == 7 ||
                      target->getCurCharacVill() != 7) &&
                     (user->getCurCharacVill() != 7 ||
                      target->getCurCharacVill() == 7))
            {
                // 同7 / 同非7 → 通过（落到组队逻辑）
            }
            else
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(0x13);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
        }
        else
        {
            // ---- 混沌袭击时间窗：4 个独立 0xfa 4-arg（PacketGuard&）错误点 ----
            int curSec = GlobalData::s_systemTime_.getCurSec();
            if (curSec < target->GetCurCharacChaosKillTime() + 600 &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    user, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
            if (curSec < user->GetCurCharacChaosKillTime() + 600 &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
            CParty* selfParty = static_cast<CParty*>(user->GetParty());
            if (selfParty != 0 &&
                selfParty->CheckMemberLastTryAssaultTime() &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
            CParty* peerParty = static_cast<CParty*>(target->GetParty());
            if (peerParty != 0 &&
                peerParty->CheckMemberLastTryAssaultTime() &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    user, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
        }

        // ---- 组队建立/加入主流程（LAB_081f12d8）----
        if (!user->CheckInParty() && !target->CheckInParty())
        {
            CParty* newParty = G_CGameManager()->GetParty();
            if (newParty == 0)
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(4);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            newParty->create_party(target);          // ORIG 忽略返回值
            newParty->setDungIndex((unsigned short)0);
            newParty->setDungDiffi(static_cast<unsigned char>(5));
            newParty->join_user(user);
            newParty->send_party_realtime_info();
            pgParty.clear();
            pgParty.put_header(0, 8);
            pgParty.put_short(user->get_unique_id() & 0xffff);
            pgParty.put_byte(0);
            pgParty.put_int(static_cast<int>(peerIdx) & 0xffff);
            pgParty.finalize(true);
            target->Send(pgParty);
            newParty->send_party_ipinfo();
            G_GameWorld()->send_party_info_to_all(newParty, 0);
            break;
        }
        if (user->CheckInParty() && !target->CheckInParty())
        {
            // [K6-DpcSigBig L5] 语义修复（接收者错，语义轴②）：ORIG 本分支
            // （081f14fe `call CUser::CheckInParty`(user) + 081f150d(0x7c=target)
            // + 081f152d `je` 同一基本块）在 0x081f1533 `mov 0xc(%ebp),%eax`
            // → 0x081f1539 `call _ZN5CUser8GetPartyEv` ⇒ 取的是 **user** 的队伍；
            // 我方旧写 `target->GetParty()`（我方 7bd2 `mov -0x80(%ebp),%eax`
            // = target）。语义：本分支 target **未组队**（!target->CheckInParty()），
            // 应把 target 加入 **user 已有的队伍**；取 target 的队伍恒得 0 ⇒
            // 必然落入 party==0 错误出口，邀请入队功能整条失效。
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party == 0)
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(4);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            if (!party->CanDoPartyActionAssalutState())
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(0x13);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            int joinErr = party->join_user(target);
            if (joinErr > 0)
            {
                party->send_quick_party_matching_result(false, 0);
                party->set_quick_party(false);
                party->send_party_realtime_info();
                pgParty.clear();
                pgParty.put_header(0, 8);
                pgParty.put_short(user->get_unique_id() & 0xffff);
                pgParty.put_byte(0);
                pgParty.put_int(static_cast<int>(peerIdx) & 0xffff);
                pgParty.finalize(true);
                target->Send(pgParty);
                party->send_party_ipinfo();
                G_GameWorld()->send_party_info_to_all(party, 2);
                break;
            }
            pgParty.clear();
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            if (joinErr == -2)
                pgParty.put_byte(0x13);
            else if (joinErr == -1)
                pgParty.put_byte(4);
            else
                pgParty.put_byte(1);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        if (!user->CheckInParty() && target->CheckInParty())
        {
            // ORIG thunk：0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、
            // 0x8294f32=GetParty(int)（movswl %ax,%esi 实参）。
            CParty* party =
                G_CGameManager()->GetParty(target->GetPartyIndex());
            if (party == 0)
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(4);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            if (!party->CanDoPartyActionAssalutState())
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(0x13);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            int joinErr = party->join_user(user);
            if (joinErr > 0)
            {
                party->send_quick_party_matching_result(false, 0);
                party->set_quick_party(false);
                party->send_party_realtime_info();
                pgParty.clear();
                pgParty.put_header(0, 8);
                pgParty.put_short(user->get_unique_id() & 0xffff);
                pgParty.put_byte(0);
                pgParty.put_int(static_cast<int>(peerIdx) & 0xffff);
                pgParty.finalize(true);
                target->Send(pgParty);
                party->send_party_ipinfo();
                G_GameWorld()->send_party_info_to_all(party, 2);
                break;
            }
            pgParty.clear();
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            if (joinErr == -2)
                pgParty.put_byte(0x13);
            else if (joinErr == -1)
                pgParty.put_byte(4);
            else
                pgParty.put_byte(1);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        break;
    }
    case 1:                                      // ---- 交易邀请应答 ----
    {
        if (user->CheckInTrade())
        {
            break;
        }
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        int err = -1;                            // ORIG -0x54
        if (target == 0)
        {
            err = 3;
        }
        else if (!target->CheckInTrade())
        {
            if (user->get_state() != target->get_state())
            {
                err = 0x13;
            }
        }
        else
        {
            err = 0x12;
        }
        if (err != -1)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(err);
            pgRes.put_byte(1);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte((char)tpErr);
            pgRes.put_byte(1);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        CTradeSpace* ts = G_CGameManager()->GetTradeSpace();
        if (ts == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(1);
            pgRes.put_byte(1);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        ts->set_traders(target, user);           // 参数序照 asm（target 先）
        pgRes.put_header(0, 8);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(1);
        pgRes.put_int(static_cast<int>(cookie));
        pgRes.put_byte(
            static_cast<int>(user->GetReliablePerson()) & 0xff);
        pgRes.put_int(
            static_cast<int>(user->getCurCharacCreateTime()));
        pgRes.finalize(true);
        target->Send(pgRes);
        pgRes.clear();
        pgRes.put_header(1, 0xb);
        pgRes.put_byte(1);
        pgRes.put_short(static_cast<int>(peerIdx) & 0xffff);
        pgRes.put_byte(1);
        pgRes.put_byte(
            static_cast<int>(target->GetReliablePerson()) & 0xff);
        pgRes.finalize(true);
        user->Send(pgRes);
        break;
    }
    case 2:                                      // ---- PvP 房间加入 ----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(3);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte((char)tpErr);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        // [DPC_SigA] P0/P183：ORIG 0x081f22de..0x081f22e6 的 GetPvpIndex 结果
        // **不经槽**（Ghidra `sVar5 = GetPvpIndex(...)` 为寄存器临时；
        // multififf 我方独有 `mov %ax,<L>`×2 而 ORIG 0 条）⇒ 单语句内联。
        PvP_Room* room =
            G_CGameManager()->GetPvp(
                static_cast<int>(target->GetPvpIndex()), user, 0);
        if (room == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(0x13);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int seatOut = 0;                         // ORIG -0xbc
        // [DPC_SigA] P0/P183：ORIG 0x081f23aa..0x081f23bd `call IsSameTeamChannelJoin`
        // → `mov %eax,(%esp)` 直入 join_room 第 3 实参（Ghidra `bVar4` 为寄存器
        // 临时，local 列表无 bool 槽）⇒ 内联。
        int joinRes = room->join_room(
            user, seatOut, G_GameWorld()->IsSameTeamChannelJoin());
        int battleMode = room->get_pvp_battle_mode();   // ORIG -0xcc
        int roomIdx = room->get_index();         // edi
        const char* accName = user->get_acc_name();
        const char* charName = user->getCurCharacName();
        CSwitchLog trace(
            __PRETTY_FUNCTION__,
            0x7a82, 0, 0);
        trace("pvp@log %s,%s,room(%d),%d,%d,%d",
              accName, charName, roomIdx, battleMode, joinRes, 0xb);
        if (joinRes > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(joinRes);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        G_CGameManager()->CheckOutParty(user, false);
        room->make_seat_info(reinterpret_cast<char*>(&pgRes), seatOut);
        G_GameWorld()->send_all(pgRes);
        G_GameWorld()->goto_pvp(user);
        break;
    }
    case 3:                                      // ---- 战争房间加入 ----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(3);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte((char)tpErr);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        // [DPC_SigA] P0/P183：ORIG 0x081f2712 前 `call GetWarRoomIndex` 结果直入
        // GetWarRoom 实参（Ghidra 该行独立且无槽）⇒ 内联。
        WarRoom* war = G_CGameManager()->GetWarRoom(target->GetWarRoomIndex());
        if (war == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(0x13);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int seatOut = 0;                         // ORIG -0xc0
        int res = war->IsJoinable(user);
        if (res > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(res);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        res = war->Join(user, seatOut);
        if (res > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(res);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        G_CGameManager()->CheckOutParty(user, false);
        war->MakeSlotInfo(reinterpret_cast<char*>(&pgRes), seatOut);
        G_GameWorld()->send_all(pgRes);
        G_GameWorld()->goto_warroom(user);
        if (war->GetWaiterCount() > 2 && war->GetState() == 0)
        {
            war->Start();
        }
        break;
    }
    case 5:
    case 7:                                      // ---- expert_job 应答（A 组）----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(3), pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(tpErr & 0xff), pgRes);
            break;
        }
        if (target->GetCurCharacExpertJobType() != 1)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0x17), pgRes);
            break;
        }
        GlobalData::s_ExpertJobMgr->OnResponsePeer(
            user, target, user, target,
            static_cast<int>(cookie),
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    case 6:
    case 8:                                      // ---- expert_job 应答（B 组，owner 互换）----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(3), pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(tpErr & 0xff), pgRes);
            break;
        }
        if (target->GetCurCharacExpertJobType() != 1)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0x17), pgRes);
            break;
        }
        GlobalData::s_ExpertJobMgr->OnResponsePeer(
            user, target, target, user,
            static_cast<int>(cookie),
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    default:
        break;
    }
    return 0;
}
