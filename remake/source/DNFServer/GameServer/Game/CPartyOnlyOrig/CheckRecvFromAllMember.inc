bool CParty::CheckRecvFromAllMember(CUser* user)
{
    set_recv_party_packet_flag(user);
    if (!recv_party_packet_all())
    {
        return false;
    }
    reset_recv_flag();
    return true;
