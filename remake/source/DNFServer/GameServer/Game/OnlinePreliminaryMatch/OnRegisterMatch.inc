int COnlinePreliminary::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_field04 == 1)
        return 1;

    if (!bMatchable_)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc7, 0x13);
        return 0;
    }

    /* ORIG @0856142d 声明初始化 `int err = 0;` 独立于 call 赋值(@+0x68)，
     * @+0x6b cmpl;setne %al;test 物化【技巧22】 */
    int err = 0;
    err = _CheckMatchable(room);
    /* ORIG @08561443 cmpl $0,-0xc; setne %al; test; je（bool 四段物化，无独立 bool 槽）
     * ——P225 while+break 形精确复现（具名 bool 会另开 -0x9 槽破坏帧）。 */
    while (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc7, (unsigned char)err);
        return 0;
    }

    if (m_field04 != 2)
        _RegisterWaitingRoom(room);

    m_field04 = 1;
    TimerStartMatching::registNextTimer(10, room->get_index(), m_authValue);
    _SendMatchStatePacket(0, 1);
    return 1;
}