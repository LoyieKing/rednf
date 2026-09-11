// [b28d-solo-CLeagueMatch] AE（TU 口径）：196=196 全等。修法=删 passwd/title
// 引用缓存局部（P183/P0，消 2 个指针槽 -0x24/-0x20）+ S27 PvPTypes.h
// PVP_ROOM_INFO::byte8 uchar→bool（消 uchar→bool 的 test/setne 2 条）。
// p167 调用点差集 CLEAR（39/39）。
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
        // [J9MgrTimerCeraItemLogin.V1CLeagueMatch 2026-09-11 · §6.5 日志文本契约]
        // ORIG 该串实字节（二进制文件偏移 0xb7a2c8 / 0xc5a23c 两份一致副本）为
        // EUC-KR `create_room fail: PVP \xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n`
        // （"슬롯반환" = U+C2AC U+B86F U+BC18 U+D658 的 cp949 编码，8 字节）。
        // 源码原写 UTF-8（ec 8a ac eb a1 af eb b0 98 ed 99 98，12 字节）⇒ 发射
        // 字节与 ORIG 不同、AE 归一化不可见（§6.5 盲区）⇒ 以 \xNN 转义复现
        // ORIG 实字节（串内容变化，函数指令/codegen 不受影响）。
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0x1fe,
            "create_room fail: PVP\xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n",
            pvp->get_index());
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
}