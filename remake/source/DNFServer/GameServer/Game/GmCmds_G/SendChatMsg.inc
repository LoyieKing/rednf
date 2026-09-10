void CUpgradeCardDebug::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, (int)strlen(msg), 2, (ENUM_NOTIPACKET)0xc);
    user->Send(packet);
