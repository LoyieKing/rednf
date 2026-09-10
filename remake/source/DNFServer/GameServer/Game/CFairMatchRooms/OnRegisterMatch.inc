int CFairMatch::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_field04 == 1)
        return 1;
    if (!bMatchable_) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199), 0x13);
        return 0;
    }
    int err = 0;
    if ((err = _CheckMatchable(room)) != 0) {
        user->SendCmdErrorPacket(
            static_cast<ENUM_CMDPACKET>(199),
            static_cast<unsigned char>(err & 0xff));
        return 0;
    }
    // ORIG 0x8563c54 实证：m_field04==2（超时兜底中）跳过等待表注册
    if (m_field04 != 2) {
        _RegisterWaitingRoom(room);
    }
    m_field04 = 1;
    TimerStartMatching::registNextTimer(10, room->get_index(),
                                        m_matchNotifyId);
    _SendMatchStatePacket(0, 1);
    return 1;
