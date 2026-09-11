// ---------------------------------------------------------------------------
// changePartyMemberInfo —— ORIG T 0x085a8d1e
// 报告：docs/class_func_reports/CParty/changePartyMemberInfo.md
// 存活且未进图者：enterMapFlag(+0x384)/装载标志(+0x388) 任一置位则清
// enterMapFlag，否则累加 BattleData.m_arr64[slot]（this+(slot+0xe0)*4+0x10）；
// 清装载标志后按 msg.m_field17 判定 FatigueUp(0)。
// ---------------------------------------------------------------------------
void CParty::changePartyMemberInfo(MSG_MOVE_MAP& info, char slot)
{
    // [b28d-51] 正向包裹（P222）：ORIG 0x085a8d38 为 `call; test %al,%al; je 尾部`
    // （4.4 对 bool callee 的 `!call` 恒加 xor$1 ⇒ 必须写正极 if 包体）。
    // 且 ORIG 每次用 slot 都重新 movsbl -0xc(%ebp)（无具名 int idx 局部，P0）。
    if (_checkValidUser(slot))
    {
        if (m_battleData.m_arr54[slot] != 1)                       // +0x380+slot
        {
            // [b28d-51] 反写（探针 z7）：ORIG 两条件 `xor$1;test;je <clear>` 同落
            // clear 岛、increment 落 fall-through ⇒ 源码须写 `==0 && ==0` 包 increment。
            if (m_battleData.m_arr58[slot] == 0 &&                 // +0x384+slot
                m_battleData.m_arr5c[slot] == 0)                   // +0x388+slot
            {
                ++bd(this).m_arr64[slot];                    // this+(slot+0xe0)*4+0x10
            }
            else
            {
                m_battleData.m_arr58[slot] = 0;
            }
        }
        m_battleData.m_arr5c[slot] = 0;

        if (canFatigueUp(info.m_field17))                    // movzbl 0x17(%eax)
            memberUser(this, slot)->FatigueUp(0);            // T 0x08655c60
    }
}