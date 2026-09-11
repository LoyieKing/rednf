// ORIG 0x8417ec2：同 StatBreakJar 形态，GetOutBuffer<MSG_STAT_PVPPLAY>。
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
}