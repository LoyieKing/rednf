bool CDisjointMachine::put_object_info(PacketGuard& p)
{
    if (m_user == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x33, 0);
        trace("disjoint machine user is NULL");
        return false;
    }
    p.put_byte(0);
    put_disjoint_machine_info(p);
    return true;
