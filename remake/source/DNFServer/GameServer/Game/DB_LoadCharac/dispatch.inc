bool DB_LoadCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    SIG_LOAD_CHARAC* sig = stream->GetOutBuffer<SIG_LOAD_CHARAC>();
    sig->m_result = 0;

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

    _sendResult(userIdx, slot, sig);

    if (G_GameWorld()->IsSchoolPvPChannel() && sig->m_characData.m_schoolNo != 0)
    {
        _UpdateSchoolCharac(sig);
    }
    return true;
