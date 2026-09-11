// ----------------------------------------------------------------------------
// GetDisjointMachineRepairCost — ORIG W 0x085d3c74 range [...cc8]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   return c->m_field0 - (c->m_field0 * count) / c->m_field4
//   （乘除均为 32 位符号运算：imul/sar/idiv 截断口径）
// ----------------------------------------------------------------------------
int STExpertJobScript::GetDisjointMachineRepairCost(int grade, int count)
{
    const stEnduranceRepairCost* c = _GetEnduranceRepairCost(
        static_cast<unsigned int>(grade));
    if (c == 0)
        return 0;
    return c->m_field0 - (c->m_field0 * count) / c->m_field4;
}