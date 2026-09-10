int EnterWarRoom(CUser* user, WarRoom* room, ENUM_CMDPACKET cmd)
{
    // [b28d-solo-WarRoom-4] ORIG 0x08201e7a：原 NEAR（残差=packet 槽基 -0x1c vs
    //   -0x20、seat -0x20 vs -0x14）⇒ **279=279 AE（离开 NEAR）**。
    //   旋钮（探针 /tmp/wr4/p.cpp f1..f5）：ORIG 槽序 seat(-0x20)/packet(-0x1c)/
    //   ret(-0x10)/party(-0xc)；4.4 对「聚合+标量」混合帧**把标量声明在聚合之后**
    //   才给 seat 深槽 -0x20、packet -0x1c（f3 命中），seat 前置(f1)/聚合化(f2)/
    //   volatile(f4)/数组(f5) 四形均不翻。故声明序 = packet, ret, party, **seat**
    //   （seat 移至队尾）；`seat = 0;` 语句仍留在 Join 前（ORIG 086be6xx 指令序）。
    PacketGuard packet;       // -0x1c
    int ret;                  // -0x10（IsJoinable 与 Join 复用同一槽，ORIG 实证）
    CParty* party;            // -0xc
    int seat;                 // -0x20（ORIG）
    // [判据⑩ 延伸 · WarRoom-4] 探针 /tmp/wr4/p.cpp f1/f3 对照：把标量 seat 置于
    //   packet 声明之后（队尾）时 4.4 才给 seat 深槽 -0x20、packet -0x1c（与 ORIG
    //   完全一致）；seat 前置/聚合化/volatile 三形均不翻（f1/f2/f4）。
    if (user->get_state() != 3)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (user->getCurCharacVill() == 7)
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        user->SendCmdErrorPacket(cmd, 0x3c);
        return 0;
    }
    if (user->IsBusyDisjointStore())
    {
        user->SendCmdErrorPacket(cmd, 0xbf);
        return 0;
    }
    if (user->IsCurCharacGhost())
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket(cmd, 0x13);
        return 0;
    }
    ret = room->IsJoinable(user);
    if (ret != 0)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (user->CheckInParty())
    {
        party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_warroom((unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    seat = 0;
    ret = room->Join(user, seat);
    if (ret > 0)
    {
        packet.put_header(1, cmd);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    if (cmd == 0x65)
    {
        packet.put_header(1, 0x65);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    room->MakeSlotInfo((char*)&packet, seat);
    G_GameWorld()->send_all(packet);
    G_GameWorld()->goto_warroom(user);
    if (room->GetWaiterCount() > 2)
    {
        if (room->GetState() == 0)
        {
            room->Start();
        }
    }
    return 1;
