void Dispatcher_SecurityCardAuthRpy::send(CUser* user, ParamBase& paramBase)
{
    SecuCardRpyParam& param = static_cast<SecuCardRpyParam&>(paramBase);
    if (param.m_4 != 0)
    {
        // 认证通过后通知交易对方断开私聊通道（0xb0 包）。
        CTradeSpace* trade = G_CGameManager()->GetTradeSpace(user->GetTradeSpace());
        if (trade != 0 && trade->is_empty() == 0)
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0xb0);
            packet.put_byte(0);
            packet.finalize(true);
            CUser* opposite = trade->GetOppositeUser(user);
            if (opposite != 0)
                opposite->Send(packet);
        }
        return;
    }
    if (param.m_5 == 0)
        return;

    PacketGuard packet;
    packet.put_header(1, 0xa7);
    packet.put_byte(0);
    packet.put_byte(param.m_5);
    if (param.m_5 == 0x87)
        packet.put_byte(user->getPad()->getFailCnt());
    packet.finalize(true);
    user->Send(packet);
