bool DB_StatPvpPlay::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    bool r;
    MSG_STAT_PVPPLAY* msg;
    msg = stream->GetOutBuffer<MSG_STAT_PVPPLAY>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
