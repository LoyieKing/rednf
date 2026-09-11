// ORIG SendChatMsg(CUser*, char*) @0x084b1e8a：strlen 后经
// GameWorld::make_packet_chat_msg(type=2, NOTI=0xc) 发给本人。
// 头已按 ORIG 两参签名声明（GameMasterCmd.h）；以真成员定义承载 ORIG 符号。
void CUpgradeCardDebug::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, (int)strlen(msg), 2, (ENUM_NOTIPACKET)0xc);
    user->Send(packet);
}