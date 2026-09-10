void CAssaultPlace::_MakeEndAssault(PacketGuard* p)
{
    p->put_header(0, 0x84);
    p->finalize(true);
