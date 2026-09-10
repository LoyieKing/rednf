bool DB_DeleteCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return 0;
    }

    bool ret;
    SIG_DELETE_CHARAC* sig = stream->GetOutBuffer<SIG_DELETE_CHARAC>();
    if (sig == 0)
    {
        return 0;
    }
    sig->m_errNo = 0;

    int curSec = GlobalData::s_systemTime_.getCurSec();

    stItemLockInfo lock[100];
    if (DB_LoadInventory::GetItemLock(sig->m_characNo, lock) == 0)
    {
        for (int i = 0; i <= 99; ++i)
        {
            if (lock[i].m_state == 1 ||
                (lock[i].m_state == 2 &&
                 lock[i].m_unlockSec + 0x3f480 > curSec))
            {
                sig->m_errNo = 3;
                if (!SendResult(userIdx, slot, sig))
                {
                    return 0;
                }
                return 1;
            }
        }
    }

    if (!DeleteGuildInfo(sig))
    {
        cMyTrace("virtual bool DB_DeleteCharac::dispatch(int, int, Stream*)",
                 0x12a7, 0)("[%s][%d] deLete charac(%d),guild_id(%d) FAIL",
                            __PRETTY_FUNCTION__, 0x12a7, sig->m_characNo,
                            sig->m_guildId);
        if (!SendResult(userIdx, slot, sig))
        {
            return 0;
        }
        return 1;
    }

    sig->m_eventFlag = 0;
    if (CheckEventCharacter40(sig))
    {
        DeleteEventCharacFlag(sig);
        sig->m_eventFlag = 1;
    }
    ret = DeleteCharac(sig);
    if (!ret)
    {
        sig->m_errNo = 1;
    }
    DeleteCharacOption(sig);
    ret = SendResult(userIdx, slot, sig);
    if (!ret)
    {
        return 0;
    }
    cMyTrace("virtual bool DB_DeleteCharac::dispatch(int, int, Stream*)",
             0x12ce, 0)("[%s][%d] deLete charac(%d),guild_id(%d)",
                        __PRETTY_FUNCTION__, 0x12ce, sig->m_characNo,
                        sig->m_guildId);
    return 1;
}
