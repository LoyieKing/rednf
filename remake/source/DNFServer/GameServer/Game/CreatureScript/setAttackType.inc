void AttackInfo::setAttackType(ENUM_ATTACKTYPE type)
{
    // N76：4.4 恒 RHS 值先载（this-> 形探针无效），ORIG LHS-this 先载序不可翻。
    m_attackType = type;
