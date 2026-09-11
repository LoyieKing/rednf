// ORIG T 0x085b3fd8（CheckRecvFromAllMember.md）：登记本人标志并轮询全员。
bool CParty::CheckRecvFromAllMember(CUser* user)
{
    set_recv_party_packet_flag(user);
    if (!recv_party_packet_all())
    {
        return false;
    }
    reset_recv_flag();
    return true;
}