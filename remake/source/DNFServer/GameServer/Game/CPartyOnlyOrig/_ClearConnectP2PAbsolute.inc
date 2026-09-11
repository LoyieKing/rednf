// ORIG T 0x0859a664（_ClearConnectP2PAbsolute.md）：除 slot 外全员中与该槽
// P2P 不通的逐个以 ENUM_PARTY_INFO_TYPE 2 离队；恒返回 1。
int CParty::_ClearConnectP2PAbsolute(int slot)   // [P9] ORIG 尾返 1
{
    for (int i = 0; i < 4; i++)
    {
        // [拆&&] ORIG 双段直分支，&& 合一 4.4 出 5 行物化岛。
        if (slot != i)
        {
            if (_IsCannotConnectP2P(slot, i))
            {
                leave_user(m_member[i].m_pUser,
                           static_cast<ENUM_PARTY_INFO_TYPE>(2));
            }
        }
    }
    return 1;
}