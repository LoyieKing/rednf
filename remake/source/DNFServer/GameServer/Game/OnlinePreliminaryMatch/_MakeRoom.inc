PvP_Room* COnlinePreliminary::_MakeRoom(CUser* user)
{
    PacketGuard pkt;
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08560fbc 先 put_header(1,0x35)，
     * 之后才 G_CGameManager()/GetPvp（原顺序反 ⇒ 序列错位）。 */
    pkt.put_header(1, 0x35);
    PvP_Room* pvp = G_CGameManager()->GetPvp();            // T 0x829507c
    if (pvp == 0)
    {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return (PvP_Room*)0;
    }

    /* [b28d-solo-OnlinePreliminaryMatch] ①删 name/pass/titleLen 布局对照局部：
     * ORIG 帧 0x6c，我们 0x9c 恰多 0x30（0x20+0x20+2 对齐）；ORIG 直接读
     * string 成员，无这些槽。②第 5 参 ORIG @08561050 `movzbl 0x10(%eax),%eax;
     * movzbl %al,%edi` = m_roomInfo.byte8（原写 true 字面量 = 真语义 bug）。
     * ③成功/失败块重排为 `if(err>0){失败}`：ORIG `jle 成功块(@0x1cc)`，
     * 失败路径 fall-through。 */
    int err = pvp->create_room(user,
                               (char)m_roomInfo.byte4,          // this[0xc]
                               const_cast<char*>(m_roomInfo.title.c_str()),
                               m_roomInfo.short6,               // this[0xe]
                               /* [b28d-solo-OnlinePreliminaryMatch] ORIG
                                * @08561050 两级 movzbl 无 test/setne（4.1 对
                                * uchar→bool 不归一化）；PvPTypes.h byte8
                                * uchar→bool 后同型直传复现。 */
                               m_roomInfo.byte8,                // this[0x10]
                               m_roomInfo.passwd.c_str(),       // this+0x14
                               (int)m_roomInfo.passwd.length(),
                               (PVP_BATTLE_MODE)2);
    if (err > 0)
    {
        // 失败路径（ORIG 0x85610ae 起，fall-through）
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        LogManager::logFormat(1, "MatchingSystem.cpp",
                              __PRETTY_FUNCTION__,
                              0x73c,
                              "create_room fail: PVP\xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n",
                              pvp->get_index());
        G_CGameManager()->PutPvp(pvp);
        return (PvP_Room*)0;
    }

    // 成功路径（ORIG 0x856115e 起，jle 落点）
    pkt.clear();                                            // 080cb8e6
    pkt.put_header(0, 2);
    pkt.put_byte(1);
    pkt.put_short(1);
    user->make_basic_info((char*)(PacketGuard*)&pkt, '\x01');  // T 0x865a44e
    pkt.finalize(true);
    user->Send(pkt);

    pkt.clear();
    pkt.put_header(0, 0x29);
    pkt.put_short(1);
    pvp->make_room_info((char*)(PacketGuard*)&pkt);            // T 0x85db7f6
    pkt.finalize(true);
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return pvp;
}