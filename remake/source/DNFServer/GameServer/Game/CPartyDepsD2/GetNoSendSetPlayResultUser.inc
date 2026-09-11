// ---------------------------------------------------------------------------
// CParty::GetNoSendSetPlayResultUser —— ORIG T 0x085b2384（报告
//   GetNoSendSetPlayResultUser.md）：未回传成员的昵称；无效返回 ""。
// ---------------------------------------------------------------------------
const char* CParty::GetNoSendSetPlayResultUser(int slot)
{
    // ORIG 0x085b2384 + Ghidra 双证：field4 **!=0** 且 pUser 非空且
    // getCurCharacR 非空才返回昵称；pUser 地址每次重算（无局部缓存 spill）。
    // [b28d-51] ORIG 0x085b23e0 第三条件出 `test; setne%al; test; je` 物化，
    // 前两条为直跳 ⇒ 源码非单一 && 链：前两条件合并 if，第三条件独立嵌套 if
    // （probe g1 裸 if(call) 在 4.4 亦产 setne；&& 链 g5/g6/g7 恒折直跳）。
    if (m_member[slot].m_pUser != 0 && m_member[slot].m_field4 != 0)
    {
        if (reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->getCurCharacR() != 0)
        {
            return reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->getCurCharacName();
        }
    }
    return "";
}