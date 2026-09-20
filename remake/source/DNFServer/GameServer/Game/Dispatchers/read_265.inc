int Dispatcher_GoldTakeIncreasingAmount::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_GOLD_TAKE_INCREASING_AMOUNT& req = *reinterpret_cast<MSG_GOLD_TAKE_INCREASING_AMOUNT*>(&msg);
    if (!packet.get_int(req.m_overSkillTime))
    {
        return LineFunc(0x38d6, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
