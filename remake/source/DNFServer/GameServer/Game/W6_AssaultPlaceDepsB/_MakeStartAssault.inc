void CAssaultPlace::_MakeStartAssault(PacketGuard* p)
{
    p->put_header(0, 0x82);
    p->finalize(true);
