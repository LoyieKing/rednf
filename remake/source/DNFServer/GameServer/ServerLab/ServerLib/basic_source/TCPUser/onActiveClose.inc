bool TCPUser::onActiveClose(unsigned int key)
{
    G_TraceLog()->sysLog(0, "call onActiveClose");
    G_TraceLog()->sysLog(5,
        "bAboutToDisconnect_=%d, bDisconnected_=%d, getSession=%x, isSyncByWorker=%d, isBindedSession=%d, pending send num=%d, pending work num=%d",
        (unsigned int)bAboutToDisconnect_, (unsigned int)bDisconnected_, getSession(),
        isActiveCloseSyncByWorker(), isBindedSession(), GetPendingSendNum(),
        GetPendingWorkNum());
    if (ddebug++ <= 0x13)
    {
        puts("call onActiveClose");
        printf("bAboutToDisconnect_=%d, bDisconnected_=%d, getSession=%p, isSyncByWorker=%d, isBindedSession=%d\n",
               (unsigned int)bAboutToDisconnect_, (unsigned int)bDisconnected_,
               getSession(), isActiveCloseSyncByWorker(), isBindedSession());
    }
    if (mUserId == key)
    {
        if (bAboutToDisconnect_ != true)
        {
            bAboutToDisconnect_ = true;
        }
        // ORIG：正条件 if/else（直 test+je + mov $1+jmp），!x 会物化 xor+test+je。
        if (bDisconnected_)
        {
            return true;
        }
        else
        {
            pApp->super_DataPools.getDataPool()->destroyTCPUser(this);
        }
    }
    return true;
