bool CTimeGate::put_object_info(PacketGuard& p)
{
    p.put_byte(2);
    p.put_byte((int)m_openState);
    return true;
