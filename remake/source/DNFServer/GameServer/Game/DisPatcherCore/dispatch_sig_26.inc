int DisPatcher_SetPVPMapIndex::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 6)
    {
        return 0;
    }
    bool flag;
    PacketGuard guard;
    short mapIdx;
    if (!packet.get_short(mapIdx))
    {
        return LineFunc(0x8650,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x8653,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    flag = false;
    if (!room->CheckPvPMapMode(user, mapIdx, flag))
    {
        guard.put_header(1, 0x3e);
        guard.put_byte(0);
        guard.put_int(0x13);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    room->SetRequestedMapNo(mapIdx);
    room->make_state_info(reinterpret_cast<char*>(&guard));
    G_GameWorld()->send_all(guard);
    // [b28d-29] P168a：ORIG 0x081f87d6 `movzbl flag; xor $0x1,%eax; test %al,%al`
    // ⇒ 原码是裸 `!flag`（bool 取反物化），`flag != true` 使 4.4 反产 test/sete。
    if (!flag && room->get_pvp_battle_mode() == 4)
    {
        int res = room->set_pvp_mode(user, 5);
        if (0 < res)
        {
            guard.put_header(1, 0x3e);
            guard.put_byte(0);
            guard.put_byte(0x13);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
        room->make_seat_info(reinterpret_cast<char*>(&guard), 0xffff);
        G_GameWorld()->send_all(guard);
    }
    return 0;
