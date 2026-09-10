bool TCPUser::onPassiveClose(char* file, int line)
{
    G_TraceLog()->sysLog(3, "call onPassiveClose : '%s', '%d'\tTotal Con = %d", file, line, pApp->super_DataPools.getDataPool()->GetTcpUserCount());
    G_TraceLog()->sysLog(3,
        "bAboutToDisconnect_=%d, bDisconnected_=%d, getSession=%x, isSyncByWorker=%d, isBindedSession=%d",
        (unsigned int)bAboutToDisconnect_, (unsigned int)bDisconnected_, this,
        isPassiveCloseSyncByWorker(), isBindedSession());
    if (bAboutToDisconnect_ != true)
    {
        bAboutToDisconnect_ = true;
    }
    // ORIG: bDisconnected_ 为真直接 return true；IsSending/IsWorking 检查在非断开分支。
    if (bDisconnected_ != false)
    {
        return true;
    }
    if (!IsSending() && !IsWorking())
    {
        pApp->super_DataPools.getDataPool()->destroyTCPUser(this);
        return true;
    }
    G_TraceLog()->sysLog(0, "onPassiveClose(): mPendingSendNum not 0");
    return false;
