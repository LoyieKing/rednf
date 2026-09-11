// ---------------------------------------------------------------------------
// ORIG T 0x82e7bbc (size 0x14d)：_DispositionUser(CUser**)
// ---------------------------------------------------------------------------
void CAssaultPlace::_DispositionUser(CUser** users)
{
    int counts[2] = { 0, 0 };
    _GetTeamCount(counts);
    CStaticDisposition disposition(users, counts);

    if (!G_GameWorld()->get_disposition(m_pos.m_x /*+0x118*/,
                                        m_pos.m_y /*+0x11c*/, &disposition))
        throw (ENUM_ERROR)0x03;

    // b28 BL6: P0——ORIG 无 flagX/flagY 槽往返，出参直绑 m_posBase(+0x110/+0x114)。
    disposition.GetFlagPoint(m_posBase.m_x, m_posBase.m_y);   // this+0x110 / +0x114 输出目标
    _SendGiveAlarm();
    _SendUserInfo();
    _SendUserUdpIpPort();
    _MoveUser(&disposition, m_pos.m_x, m_pos.m_y);
    _MoveAssaultPlace();
}