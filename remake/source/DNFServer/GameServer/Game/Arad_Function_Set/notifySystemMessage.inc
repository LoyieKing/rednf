// ---- notifySystemMessage @ 0x08197d00（objdump 转写【推断】）-----------------
void notifySystemMessage(CUser* user, char* msg)
{
    if (user == 0)          // 0x8197d08: cmpl $0x0,0x8(%ebp) → je ret
        return;
    if (msg == 0)           // 0x8197d12: cmpl $0x0,0xc(%ebp) → je ret
        return;

    int len = static_cast<int>(strlen(msg));   // 0x8197d22 strlen

    PacketGuard packet;                        // -0x18 栈对象
    // type=0、notify=(ENUM_NOTIPACKET)0xc（ORIG movl $0xc,0x18(%esp)）
    G_GameWorld()->make_packet_chat_msg(
        packet, user, msg, len, (char)0, (ENUM_NOTIPACKET)0xc);
    user->Send(packet);
}