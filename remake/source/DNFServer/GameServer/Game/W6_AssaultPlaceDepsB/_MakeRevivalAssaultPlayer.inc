void CAssaultPlace::_MakeRevivalAssaultPlayer(PacketGuard* p, int idx)
{
    p->put_header(0, 0x85);
    p->put_short(idx);
    p->finalize(true);
