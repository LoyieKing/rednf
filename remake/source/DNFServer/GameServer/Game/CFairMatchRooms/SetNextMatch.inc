void CFairMatch::SetNextMatch(IMatch* next)
{
    // [B24D判词 N] 语义正确：m_fieldC0=(int)next；diff=mov 0xc(%ebp),%edx 与
    // mov 0x8(%ebp),%eax 装载序互换（GCC4.1 vs 4.4），7/7 纯 lowering，语义等价。
    m_fieldC0 = next;
}