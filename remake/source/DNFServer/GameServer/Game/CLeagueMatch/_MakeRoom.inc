PvP_Room* CLeagueMatch::_MakeRoom(CUser* user)   // ORIG T 0x0855d572
{
    PacketGuard pkt;
    pkt.put_header(1, 0x35);
    PvP_Room* pvp = G_CGameManager()->GetPvp();
    if (pvp == 0) {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return 0;
    }

    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 0x0855d5xx 每处重算
    // `mov 0x8(%ebp),%eax; add $0x14/$0x8,%eax`（无 passwd/title 引用缓存槽）⇒ 删引用
    // 局部改直接成员访问，消 2 个指针槽（-0x24/-0x20）。
    int err = pvp->create_room(user, m_roomInfo.byte4,
                               const_cast<char*>(m_roomInfo.title.c_str()), m_roomInfo.short6,
                               m_roomInfo.byte8 != 0, m_roomInfo.passwd.c_str(),
                               static_cast<int>(m_roomInfo.passwd.length()),
                               static_cast<PVP_BATTLE_MODE>(2));
    if (err >= 1) {
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0x1fe,
            "create_room fail: PVP슬롯반환 ID[%d]\n", pvp->get_index());
        G_CGameManager()->PutPvp(pvp);
        return 0;
    }

    pkt.clear();
    pkt.put_header(0, 2);
    pkt.put_byte(1);
    pkt.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&pkt), 1);
    pkt.finalize(true);
    user->Send(pkt);

    pkt.clear();
    pkt.put_header(0, 0x29);
    pkt.put_short(1);
    pvp->make_room_info(reinterpret_cast<char*>(&pkt));
    pkt.finalize(true);
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return pvp;
