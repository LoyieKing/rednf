// ============================================================================
// DB_DeleteCharac::dispatch @0x08402c38
// 基类 dispatch 通过 → GetOutBuffer<SIG_DELETE_CHARAC>（空 → 0）→
// +0x8 清零 → getCurSec → stItemLockInfo[100] 自动构造 → GetItemLock
// 成功则扫描 100 项：state==1 或 (state==2 且 unlockSec+0x3f480>curSec)
// → +0x8=3 + SendResult 返 1。DeleteGuildInfo 失败 → cMyTrace 0x12a7 +
// SendResult；成功 → +0x10=0，CheckEventCharacter40 为真则
// DeleteEventCharacFlag + +0x10=1；DeleteCharac 失败 +0x8=1；
// DeleteCharacOption；SendResult 失败 → 0；成功 cMyTrace 0x12ce 返 1。
// ============================================================================
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
