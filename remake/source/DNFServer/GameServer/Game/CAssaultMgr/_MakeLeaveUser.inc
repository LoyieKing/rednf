void CAssaultPlace::_MakeLeaveUser(PacketGuard* p, int index)
{
    p->put_header(0, 0x87);
    p->put_short((int)m_assaulters[index].GetUserUniqueId());
    p->finalize(true);
