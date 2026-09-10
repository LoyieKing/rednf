bool CParty::recv_party_packet_all()
{
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // N96 清绑：ORIG call; xor$1; test; jne 岛——4.4 对 uchar callee 恒
        // setXX 物化（P168a④ 在册），裸形取最小差。
        if (!reinterpret_cast<CPartyRecvFlagView*>(this)
                 ->m_recvFlag.IsRecv(i))
            return false;
    }
    return true;
