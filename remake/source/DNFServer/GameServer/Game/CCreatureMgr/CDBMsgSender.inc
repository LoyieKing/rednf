// ORIG C1 @ 0x833f8ae：仅清 m_stream（offset 0）后委托 Init；m_user 由 Init 写入。
CDBMsgSender::CDBMsgSender(ENUM_INTERNALPACKET type, CUser* user)
    : m_stream(0)
{
    Init(type, user);
}
