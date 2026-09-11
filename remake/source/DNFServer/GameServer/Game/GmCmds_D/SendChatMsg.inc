// ORIG SendChatMsg 0x084b15c4：聊天包(type=2, notify=0xc) 发起者私聊回显。
void CIndependentDrop::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    G_GameWorld()->make_packet_chat_msg(packet, user, msg,
                                        static_cast<int>(strlen(msg)), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
}