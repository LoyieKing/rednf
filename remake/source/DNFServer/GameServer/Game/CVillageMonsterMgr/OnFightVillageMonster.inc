// ============================================================================
// CVillageMonster::OnFightVillageMonster —— ORIG T 0x86b3240
//   m_state!=0 → 0x13；距怪物 > 0x12c（x 或 y）→ 0x63；否则状态置 1、
//   记录 partyIndex，回填 m_field20/m_field24 到 out1/out2，返回 0。
// ============================================================================
int CVillageMonster::OnFightVillageMonster(CUser* user, int& out1, int& out2)
{
    if (m_state != 0)
        return 0x13;
    // [b28-L11 清违例] 原 register bool asm("eax") 违反 P0/N96；还原 P133
    // 内联短路判定形（探针 dl17p/p2.cpp g4：内联短路 4.4 复现直分支）。
    if (abs((int)user->get_posX() - m_field04) > MONSTER_MOVE_PIXEL ||
        abs((int)user->get_posY() - m_field08) > MONSTER_MOVE_PIXEL)
        return 0x63;
    OnChangeState(ENUM_VILLAGE_MONSTER_STATE_1);
    m_partyIndex = (int)user->GetPartyIndex();
    out1 = m_field20;
    out2 = m_field24;
    return 0;
}