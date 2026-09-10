void CDeathTower::attachParty(CParty* party)
{
    m_party = party;
    // ORIG 0x8462a3c/0x8462a45：i 双次置 0 直落 -0xc（范式8）；int 计数 setle。
    int i = 0;
    i = 0;
    for (; i <= 3; ++i)
    {
        // ORIG 0x8462a45：receiver 走 this->m_party reload（mov 0x8,%eax; mov(%eax),%eax），
        // 非 party 形参槽。
        if (m_party->checkValidUser(i))
        {
            // ORIG 0x8462a61：movswl %ax,%ebx sext 直落 %ebx（P200：短整表达式
            // 作普通实参直传并跨 get_user call 存活，禁 asm 绑定/中间局部）。
            m_party->get_user(i)->setDeathTowerIndex((short)m_idx);
        }
    }
    setTowerState((ENUM_TOWER_STATE)1);
