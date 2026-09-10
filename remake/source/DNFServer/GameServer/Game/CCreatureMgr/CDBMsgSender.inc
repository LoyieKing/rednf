CDBMsgSender::CDBMsgSender(ENUM_INTERNALPACKET type, CUser* user)
    : m_stream(0)
{
    Init(type, user);
