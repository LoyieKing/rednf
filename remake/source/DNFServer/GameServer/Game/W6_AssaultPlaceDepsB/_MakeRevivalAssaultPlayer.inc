// ORIG T 0x82e8978 _MakeRevivalAssaultPlayer(PacketGuard*, int)（ORIG 未用 this）
void CAssaultPlace::_MakeRevivalAssaultPlayer(PacketGuard* p, int idx)
{
    p->put_header(0, 0x85);
    p->put_short(idx);
    p->finalize(true);
}