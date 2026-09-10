void TCPUser::postDisconnected(int nReason)
{
    bAboutToDisconnect_ = true;
    G_TraceLog()->sysLog(3, "In postDisconnected ='%d'", nReason);
    nReason_ = nReason;
