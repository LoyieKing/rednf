// ---------------------------------------------------------------------------
// ORIG T 0x82e88a0 (size 0x35)：_MakeEndAssault(PacketGuard*) —— 裸包 0x84
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeEndAssault(PacketGuard* p)
{
    p->put_header(0, 0x84);
    p->finalize(true);
}