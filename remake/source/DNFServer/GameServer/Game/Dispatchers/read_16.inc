int Dispatcher_BingoQuiz::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BINGO_QUIZ& req = *reinterpret_cast<MSG_BINGO_QUIZ*>(&msg);
    int value = 0;
    if (!packet.get_int(value))
    {
        return LineFunc(0x1de, __PRETTY_FUNCTION__, 0, 0);
    }
    req.m_0d = value;
    return 0;
}
