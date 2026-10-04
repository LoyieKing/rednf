// ORIG T 0x085b3f72（recv_party_packet_all.md）：全员均已回包才返回 true。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE（+0x1dc 视图具名 receiver，
// 同 set_recv_party_packet_flag 修法）。
// b28d-45 recv_party_packet_all 0x085b3f72：已修 P248 档①（CPartyResultRecvFlag::
// IsRecv uchar→bool，0x085b3fc0 xor 段对齐）。
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
        if (!this
                 ->m_recvFlag.IsRecv(i))
            return false;
    }
    return true;
}