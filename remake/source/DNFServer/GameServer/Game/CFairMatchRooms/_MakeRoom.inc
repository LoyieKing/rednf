// _MakeRoom ORIG T 0x8563692 报告 _MakeRoom.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P222 反向布局（ORIG
// `jle <失败尾块>` + result>=1 成功路径直落；改写 if(result>=1){成功}else{失败}）
// + P162；call 差集 45=45。fresh ndiff=0（230=230）。
PvP_Room* CFairMatch::_MakeRoom(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(1, 0x35);

    PvP_Room* room = G_CGameManager()->GetPvp();
    if (room == 0) {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return 0;
    }

    // ORIG 实参序列：byte=m_roomInfo.byte4、name=title.c_str()、
    // short=short6、bool=byte8、password=passwd.c_str()/length()、mode=2
    const int result = room->create_room(
        user,
        static_cast<char>(m_roomInfo.byte4),
        const_cast<char*>(m_roomInfo.title.c_str()),
        m_roomInfo.short6,
        m_roomInfo.byte8,
        m_roomInfo.passwd.c_str(),
        static_cast<int>(m_roomInfo.passwd.length()),
        static_cast<PVP_BATTLE_MODE>(2));
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    CSwitchLog("PvP_Room* fair_pvp::CFairMatch::_MakeRoom(CUser*)",
               0xba0, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                            user->get_acc_name(), user->getCurCharacName(),
                            room->get_index(), room->get_pvp_battle_mode(), result);

    // b28d-solo-CFairMatchRooms：P222 反向布局 —— ORIG `jle <失败尾块>` +
    // result>=1 成功路径直落（logFormat 0xba9 + PutPvp + return 0），失败块在尾。
    if (result >= 1) {
        pkt.put_byte(0);
        // P162：ORIG 直接传 int（无 movzbl）。
        pkt.put_byte(result);
        pkt.finalize(true);
        user->Send(pkt);
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0xba9,
            // [b28f-SweepCFairMatch] §6.5 日志文本契约（AE 盲区）：ORIG .rodata
            // 0x08ca2244 实字节为 EUC-KR `bd bd b7 d4 b9 dd c8 af`（"슬롯반환"），
            // 原 UTF-8 字面量发射字节不同（运行期日志乱码）⇒ 改 \xNN 转义。
            "create_room fail: PVP\xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n",
            room->get_index());
        G_CGameManager()->PutPvp(room);
        return 0;
    } else {
        pkt.clear();
        pkt.put_header(0, 2);
        pkt.put_byte(1);
        pkt.put_short(1);
        // ORIG 0x85661b4 实证 make_basic_info 直接写 pkt（无独立缓冲/put_binary；
        // make_basic_info 内部将 buf reinterpret_cast 为 InterfacePacketBuf）
        user->make_basic_info(reinterpret_cast<char*>(&pkt), 1);
        pkt.finalize(true);
        user->Send(pkt);

        pkt.clear();
        pkt.put_header(0, 0x29);
        pkt.put_short(1);
        room->make_room_info(reinterpret_cast<char*>(&pkt));
        pkt.finalize(true);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
        return room;
    }
}