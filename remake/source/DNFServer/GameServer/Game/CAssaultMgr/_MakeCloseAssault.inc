void CAssaultPlace::_MakeCloseAssault(PacketGuard* p, int tick)
{
    p->put_header(0, 0x89);
    p->put_byte(tick);
    p->finalize(true);
