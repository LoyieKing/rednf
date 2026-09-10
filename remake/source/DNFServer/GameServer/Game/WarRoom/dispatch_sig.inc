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
    createRet = room->Create(user, (char*)"임시", 0x1770);
    if (createRet > 0)
    {
        pg.put_byte(0);
        pg.put_byte(createRet);
        pg.finalize(true);
        user->Send(pg);
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__,
                              0x976d, "create_room fail: 쟁역 방 생성 실패 ID[%d]\n",
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
