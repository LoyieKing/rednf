void CAssaultPlace::_MakeDieAssaultPlayer(PacketGuard* p, int idx)
{
    p->put_header(0, 0x83);
    p->put_short(m_assaulters[idx].GetUserUniqueId() & 0xffff);
    p->finalize(true);
