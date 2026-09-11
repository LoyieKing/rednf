// ORIG T 0x82e886a _MakeStartAssault(PacketGuard*)
void CAssaultPlace::_MakeStartAssault(PacketGuard* p)
{
    p->put_header(0, 0x82);
    p->finalize(true);
}