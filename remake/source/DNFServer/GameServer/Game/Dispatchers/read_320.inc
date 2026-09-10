int Dispatcher_RefundSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REFUND_SKILL& req = reinterpret_cast<MSG_REFUND_SKILL&>(msg);
    if (!packet.get_int(req.m_0d))  // 技能 id
        return LineFunc(0x48e5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_11)))  // 技能树选择（signed byte 读入）
        return LineFunc(0x48e7, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_12))  // 树种类
        return LineFunc(0x48e9, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_13))  // 退款数量
        return LineFunc(0x48eb, __PRETTY_FUNCTION__, 0, 0);
    return 0;
