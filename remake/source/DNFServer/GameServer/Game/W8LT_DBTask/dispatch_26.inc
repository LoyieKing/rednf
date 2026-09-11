// ORIG 0x8417cde：同 StatBreakJar 形态，GetOutBuffer<MSG_STAT_DUNGEONCLEAR>。
bool DB_StatDungeonClear::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    bool r;
    MSG_STAT_DUNGEONCLEAR* msg;
    msg = stream->GetOutBuffer<MSG_STAT_DUNGEONCLEAR>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
}