// ============================================================================
// DisPatcher_MakeWarRoomTemp 族（ORIG vtable @0x8bd8cc8，8 槽）
//   dispatch_sig 0x08201b32（报告 dispatch_sig.md 全文转写）
//   read 0x082335f8 / process 0x08233602（objdump 转写：恒定返回 0）
//   D1/D2 0x082431aa / D0 0x082431da（objdump 转写：vptr + 基类析构 + delete 守卫）
// ============================================================================
int DisPatcher_MakeWarRoomTemp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    if (user->get_state() != 3)
        return 0;
    PacketGuard pg;
    WarRoom* room = G_CGameManager()->GetWarRoom();
    int createRet;
    CParty* party;
    pg.put_header(1, 0x5c);
    if (room == 0)
    {
        pg.put_byte(0);
        pg.put_byte(4);
        pg.finalize(true);
        user->Send(pg);
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
    createRet = room->Create(user, (char*)"\xc0\xd3\xbd\xc3", 0x1770);
    if (createRet > 0)
    {
        pg.put_byte(0);
        pg.put_byte(createRet);
        pg.finalize(true);
        user->Send(pg);
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__,
                              // [S3-W8 2026-09-11 §6.5.1] ORIG 串为 EUC-KR 字节
                              //   `\xc0\xef\xbf\xaa \xb9\xe6 \xbb\xfd\xbc\xba \xbd\xc7\xc6\xd0`，
                              //   旧码 UTF-8 直写 ⇒ 二进制字节不符（源仍 UTF-8，用 \xNN 表示）。
                              0x976d,
                              "create_room fail: \xc0\xef\xbf\xaa \xb9\xe6 \xbb\xfd\xbc\xba "
                              "\xbd\xc7\xc6\xd0 ID[%d]\n",
                              room->GetIndex());
        G_CGameManager()->PutWarRoom(room);
        return 0;
    }
    pg.clear();
    pg.put_header(0, 2);
    pg.put_byte(1);
    pg.put_short(1);
    user->make_basic_info((char*)&pg, 1);
    pg.finalize(true);
    user->Send(pg);
    pg.clear();
    pg.put_header(0, 0x57);
    pg.put_short(1);
    room->MakeRoomInfo((char*)&pg);
    pg.finalize(true);
    G_GameWorld()->send_all(pg);
    G_GameWorld()->goto_warroom(user);
    return 0;
}