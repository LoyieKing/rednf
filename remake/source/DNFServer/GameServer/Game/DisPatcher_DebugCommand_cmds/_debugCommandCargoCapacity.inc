// ===========================================================================
// 21. _debugCommandCargoCapacity —— 0x0858fdc8
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE + 真语义 bug 修复：派生类
// `static const unsigned int packetSize=0x14` 遮蔽 PacketHeader::packetSize
// 成员（+0x2，ctor 写 0x1e）⇒ 发送长度少 0xa 字节。改 `pkt.PacketHeader::packetSize`
// 后复现 ORIG `movzwl -0x24(%ebp),%eax`；receiver 改链式（P52）。
bool DisPatcher_DebugCommand::_debugCommandCargoCapacity(CUser* user,
                                                         MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    Packet_Guild_Guild_Cargo_Upgrade pkt;                       // ctor(0x713,0x1e)
    pkt.m_guildKey = user->get_charac_guildkey();               // pkt+0xa
    pkt.m_charNo = user->getCurCharacNo();                      // pkt+0xe
    pkt.m_curCapacity = user->GetGuildCargoCapacity();          // pkt+0x12 当前容量
    pkt.m_newCapacity = msg.m_arg;                              // pkt+0x16 新容量=msg[0xe]
    // P52 链式 receiver；长度取 PacketHeader 基类 ushort 成员（ORIG 0x0858fe3x
    // `movzwl -0x24(%ebp),%eax` 读 pkt+2=0x1e）；派生类同名 static const 遮蔽会
    // 折叠成立即数 0x14 —— 真语义 bug（少发 0xa 字节）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))->SendTcpPacket(
            reinterpret_cast<char*>(&pkt),
            pkt.PacketHeader::packetSize);                      // size = *(u16*)(pkt+2)
    return true;
}