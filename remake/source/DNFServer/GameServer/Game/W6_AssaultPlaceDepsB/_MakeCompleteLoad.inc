// ORIG T 0x82e8810 _MakeCompleteLoad(PacketGuard*, int)
void CAssaultPlace::_MakeCompleteLoad(PacketGuard* p, int idx)
{
    p->put_header(0, 0x81);
    // ORIG 无 uid 局部(P0 右值直传): GetUserUniqueId 结果 movzwl 直入 put_short
    p->put_short(m_assaulters[idx].GetUserUniqueId() & 0xffff);
    p->finalize(true);
}