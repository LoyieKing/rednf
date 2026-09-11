// ==== Dispatcher_TournamentRewardSelect::read (ORIG 0x081dffda) ====
int Dispatcher_TournamentRewardSelect::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    // get_byte(char&) 直写 msg+0xd；随后域检验 [0,1]（js/cmp $1,jle 形态）
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x52fd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (static_cast<char>(req.m_0d) < 0 || static_cast<char>(req.m_0d) > 1)
    {
        return LineFunc(0x5300,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0e)))
    {
        return LineFunc(0x5303,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (static_cast<char>(req.m_0e) < 0 || static_cast<char>(req.m_0e) > 1)
    {
        return LineFunc(0x5306,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}