// ORIG SendChatMsg 0x084aeac6：make_packet_chat_msg(type=2,len=12) + Send。
void CBoosterTest::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    // ORIG：strlen 先求值物化 %ebx 跨 G_GameWorld call；gw 经 eax 直通 receiver，
    // 无具名 gw 槽（Ghidra C 顺序 strlen→G_GameWorld→make_packet_chat_msg）。
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, static_cast<int>(strlen(msg)), 2,
        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
}