// b28d-solo-CParty: N 封顶 —— N82 帧 0x8c/0x9c + callee-saved %edi/%esi 角色差；277 vs 270
bool CParty::_checkValidUser(int idx)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b4d12
    if (m_member[idx].m_pUser != 0)
    {
        if ((int)m_member[idx].m_pUser->GetUID() == m_member[idx].m_field8)
        {
            if (reinterpret_cast<CUserCharacInfo*>(m_member[idx].m_pUser)->getCurCharacR() != 0)
            {
                return 1;
            }
        }
        // [G7-sub] ORIG 0x085b4db0..0x085b4e1a：仅 cState 落槽（-0x74），
        // pIndex→%edi / dungIndex→%esi / manager→%ebx **全程无槽**（P0 ⇒ 原码无具名）
        // 且三者都在 cMyTrace ctor（0x085b4df4）**之前**求值保活 ⇒ 原码是
        // 全表达式临时链（P227/P87）。旧码 4 个具名量 ⇒ 多 3 槽（-0x34/-0x2e/-0x2c），
        // 且 ctor 先于实参求值（与 ORIG 反）。
        int cState = (char)m_state;
        cMyTrace(__PRETTY_FUNCTION__, 0x37cf, 5)(
            "[PARTYPOOL] m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
            m_manager, (unsigned int)m_dungIndex, m_partyIndex, cState);
        for (int i = 0; i <= 3; ++i)
        {
            if (m_member[i].m_pUser == 0)
            {
                // ORIG 0x085b4f38..0x085b4f8b：m_field8 在 ctor 前求值入 %ebx（无槽）
                // ⇒ 全表达式临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0x37e5, 5)(
                    "User[%d] NULL uid : %d", i, m_member[i].m_field8);
            }
            else
            {
                // ORIG 0x085b4e4a..0x085b4f31：6 个实参在 ctor（0x085b4efd）前求值，
                // 值全在 callee-saved/寄存器（get_state→%edi、GetUID→%esi、
                // m_pUser→%ebx；m_field8/-0x70 与 getCurCharacNo/-0x6c 是 4.1 的实参
                // spill，非源码具名）⇒ 全表达式临时链（P227）。
                cMyTrace(__PRETTY_FUNCTION__, 0x37da, 5)(
                    "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
                    i, m_member[i].m_pUser, m_member[i].m_pUser->get_state(),
                    (int)m_member[i].m_pUser->GetUID(), m_member[i].m_field8,
                    m_member[i].m_pUser->getCurCharacNo());
            }
        }
        // ORIG 0x085b4fbd..0x085b5018：`GetUID → %esi`、`get_acc_id` 直传
        // NumberToString（无 accId 槽）、结果 → %ebx，均在 ctor（0x085b5013）之前
        // ⇒ 全表达式临时链，无具名局部（P0）。
        cMyTrace(__PRETTY_FUNCTION__, 0x37ed, 5)(
            "User missmatched in party (m_id: %s)(uid: %d)",
            NumberToString(m_member[idx].m_pUser->get_acc_id(), 0),
            (int)m_member[idx].m_pUser->GetUID());
        m_member[idx].m_pUser->DisConnSig((DISCONN_SIG)0x17, false, 0);
        m_member[idx].Init();
    }
    return 0;
}