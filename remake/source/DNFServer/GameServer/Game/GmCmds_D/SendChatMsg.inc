void CIndependentDrop::SendChatMsg(CUser* user, char* msg)
{
    PacketGuard packet;
    G_GameWorld()->make_packet_chat_msg(packet, user, msg,
                                        static_cast<int>(strlen(msg)), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));
    user->Send(packet);
