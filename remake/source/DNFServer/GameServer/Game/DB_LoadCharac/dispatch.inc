// ============================================================================
// DB_LoadCharac::dispatch @0x08408a3c
// DBDispatcher::dispatch 先决（失败返 0）→ GetOutBuffer<SIG_LOAD_CHARAC>()，
// +0x08(m_result) 清零。try{ _getCharacInfo 失败 throw 1；_getCharacStat
// 失败 throw 3；事件 0x19b is_eventing 且 _LoadEventBringUpInfo 失败
// throw 5 } catch(int line){ m_result=line }。_sendResult 后按
// IsSchoolPvPChannel && schoolNo(+0x682)!=0 调 _UpdateSchoolCharac。返 1。
// ============================================================================
bool DB_LoadCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    SIG_LOAD_CHARAC* sig = stream->GetOutBuffer<SIG_LOAD_CHARAC>();
    sig->m_result = 0;
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG
    cMyTrace(__PRETTY_FUNCTION__, 0x2348, 0)(
        "[CHARLIST_DIAG] db_load_start acc=%u charac=%u user_idx=%d slot=%d",
        sig->m_accId, sig->m_characNo, userIdx, slot);
#endif

    try
    {
        if (!_getCharacInfo(sig))
        {
            throw 1;
        }
        if (!_getCharacStat(sig))
        {
            throw 3;
        }
        if (((EventClassify::CEventScriptMng*)G_CDataManager()
                 ->get_event_script_mng())
                ->is_eventing(0x19b))
        {
            if (!_LoadEventBringUpInfo(sig))
            {
                throw 5;
            }
        }
    }
    catch (int line)
    {
        sig->m_result = line;
    }

#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG
    cMyTrace(__PRETTY_FUNCTION__, 0x2349, 0)(
        "[CHARLIST_DIAG] db_load_done acc=%u charac=%u result=%d",
        sig->m_accId, sig->m_characNo, sig->m_result);
#endif

    _sendResult(userIdx, slot, sig);

    if (G_GameWorld()->IsSchoolPvPChannel() && sig->m_characData.m_schoolNo != 0)
    {
        _UpdateSchoolCharac(sig);
    }
    return true;
}