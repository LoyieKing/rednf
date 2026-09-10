void CUserCharacInfo::SetOverTradeGoldCount(unsigned int count)
{
    // ORIG：`mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0xd0(%eax)`
    // LHS(this) 先载。字段已改 unsigned（头同步）与形参 Ej 同型直赋无转换节点，
    // 4.4 出 LHS 先=ORIG 同形（N76 翻案：无转换→LHS 先，有转换→RHS 先）。
    m_overTradeGoldCount = count;
