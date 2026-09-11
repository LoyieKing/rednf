// ORIG T 0x82e88d6：_MakeCloseAssault —— 包 0x89 + tick。
void CAssaultPlace::_MakeCloseAssault(PacketGuard* p, int tick)
{
    p->put_header(0, 0x89);
    p->put_byte(tick);
    p->finalize(true);
}