// ---------------------------------------------------------------------------
// ORIG T 0x82e90a0 (size 0x6b)：_MakeChangeHp(PacketGuard*, int, int)
// 包 0x88：uid(short)+hp(int)
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeChangeHp(PacketGuard* p, int index, int hp)
{
    p->put_header(0, 0x88);
    p->put_short((int)(m_assaulters[index].GetUserUniqueId() & 0xffff));
    p->put_int(hp);
    p->finalize(true);
}