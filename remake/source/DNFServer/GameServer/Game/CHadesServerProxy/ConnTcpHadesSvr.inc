bool CHadesServerProxy::ConnTcpHadesSvr()
{
    BaseServerProxy::Init();
    BaseServerProxy::SetSocketState(ENUM_SOCKET_STATE_CONNECTED);
    m_connSec = GlobalData::s_systemTime_.getCurSec();
    return 1;
