TCPUser::TCPUser()
    : mRecvBuffer(G_Script()->findIntValue(0, 0xc), G_Script()->findIntValue(0, 0xd))
{
    bDisconnected_ = false;
    bAboutToDisconnect_ = false;
    user_count = user_count + 1;
    mUserId = user_count;
    pSession = 0;
    mbBindedSession = false;
    mPendingWork = 0;
    mPendingSend = 0;
    mMaxPacketSize = G_Script()->findIntValue(0, 0xd);
