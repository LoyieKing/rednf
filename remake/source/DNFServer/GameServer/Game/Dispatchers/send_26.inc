void Dispatcher_Req_Urgent_Quest::send(CUser* user, ParamBase& param)
{
    const ReqResult& res = static_cast<const ReqResult&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c6);
    packet.put_byte(res.m_found != 0 ? 1 : 0);
    if (res.m_found)
    {
        packet.put_short(res.m_questIdx);
    }
    else
    {
        packet.put_byte(res.m_errorCode);
    }
    packet.finalize(true);
    user->Send(packet);
