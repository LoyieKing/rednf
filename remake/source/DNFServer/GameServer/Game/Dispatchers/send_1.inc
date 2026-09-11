void Dispatcher_BingoQuiz::send(CUser* user, ParamBase& param)
{
    stBingoQuiz& out = static_cast<stBingoQuiz&>(param);
    // ORIG 各分支独立 PacketGuard（-0x18 / -0x24 分别构造）。
    if (out.m_result == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x203);
        packet.put_byte(1);
        packet.put_int((int)out.m_bingoData);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x203);
        packet.put_byte(0);
        packet.put_byte(out.m_result);
        packet.put_int((int)out.m_bingoData);
        packet.finalize(true);
        user->Send(packet);
    }
}