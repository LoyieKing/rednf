void STRDCharacterStatus::Clear()
{
    m_hpMax = 0;
    m_mpMax = 0;
    m_physAttack = 0;
    m_physDefense = 0;
    m_magAttack = 0;
    m_magDefense = 0;
    int i = 0;
    for (i = 0; i < 4; ++i)   // ORIG 双 init（声明=0 + for 独立 init，同槽 -0x4）
        m_elemResist[i] = 0;
    for (i = 0; i < 17; ++i)
        m_activeResist[i] = 0;
    m_inventoryLimit = 0;
    m_hpRegen = 0;
    m_mpRegen = 0;
    m_moveSpeed = 0;
    m_attackSpeed = 0;
    m_castSpeed = 0;
    m_hitRecovery = 0;
    m_jumpPower = 0;
    m_weight = 0;
    m_field52 = 0;
    m_field53 = 0.0f;   // b28c-28：float 位模式零 ⇒ ORIG `mov $0,%edx; mov %edx,disp`
