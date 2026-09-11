// ORIG T 0x0859a6dc（_ClearConnectP2PRelative.md）：仅扫描槽位 2..3，
// 与 param 的 P2P 点数相等者离队；恒返回 1。
int CParty::_ClearConnectP2PRelative(int slot)
{
    for (int i = 2; i < 4; i++)
    {
        if (_GetConnectP2PPoint(i) == slot)
        {
            leave_user(m_member[i].m_pUser,
                       static_cast<ENUM_PARTY_INFO_TYPE>(2));
        }
    }
    return 1;
}