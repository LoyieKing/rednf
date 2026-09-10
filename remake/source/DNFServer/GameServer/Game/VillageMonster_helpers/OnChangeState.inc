void CVillageMonster::OnChangeState(ENUM_VILLAGE_MONSTER_STATE state)
{
    // P95：ORIG 0x86b35a0 首句 store 先载 this 后载 state；4.4 直成员赋值先备值。
    // 具名视图复现（仅表达布局；m_state = CVillageMonster +0x14）。
    struct CVMonsterStateView
    {
        char m_head[0x14];
        int  m_state;
    };
    ((CVMonsterStateView*)this)->m_state = state;
    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0xf0);
    pg.put_byte(m_state);
    pg.put_short(m_id);
    pg.finalize(true);
    m_area->SendPacket(pg);
