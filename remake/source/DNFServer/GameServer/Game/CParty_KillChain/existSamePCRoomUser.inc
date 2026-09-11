// ============================================================================
// existSamePCRoomUser —— ORIG T 0x085b67ca。
// 统计 PC 房号==pcbangNo 且非 0 的有效成员数，>=2 → true。
// ============================================================================
bool CParty::existSamePCRoomUser(unsigned int pcbangNo)
{
    int count = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            // ORIG 两次调用 GetPCRoomNo：uVar3==param_1 且 iVar4!=0。
            if (static_cast<unsigned int>(
                    m_member[i].m_pUser->GetPCRoomNo()) == pcbangNo
                && static_cast<unsigned int>(
                    m_member[i].m_pUser->GetPCRoomNo()) != 0)
            {
                count += 1;
                if (count > 1)
                    return true;
            }
        }
    }
    return false;
}