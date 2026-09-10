bool DB_UpdateQuest::dispatch(int userIdx, int slot, Stream* stream)
{
    bool bResult = false;
    SIG_UPDATE_QUEST* sig = stream->GetOutBuffer<SIG_UPDATE_QUEST>();
    if (sig == 0)
        return false;
    _updateQuests(sig);
    return bResult;
