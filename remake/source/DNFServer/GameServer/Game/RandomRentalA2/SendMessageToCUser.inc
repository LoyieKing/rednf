void RegenerationROI::SendMessageToCUser(CUser* user, const char* msg)
{
    // ORIG 0x085f853c：直测 bool（test;je 早退），真路径 fall-through ⇒ 正向
    // if 包裹（P1/P19 块序，消 4.4 的 xor;jne + 尾 jmp）。
    if (user->isGMUser())
    {
        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int((int)strlen(msg));
        packet.put_str(msg, (int)strlen(msg));
        packet.finalize(true);
        user->Send(packet);
    }
