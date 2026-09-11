// ----------------------------------------------------------------------------
// GetDisjointMachineRepair — ORIG W 0x085d3cc8 range [...d3a]
//   c = _GetEnduranceRepairCost(grade)；if (!c) return 0；
//   div = c->m_field0 / c->m_field4；if (div <= 0) return 0；
//   outCost = endurance - endurance % div；return endurance / div
// ----------------------------------------------------------------------------
int STExpertJobScript::GetDisjointMachineRepair(int grade, int endurance,
                                                int& outCost)
{
    const stEnduranceRepairCost* c = _GetEnduranceRepairCost(
        static_cast<unsigned int>(grade));
    if (c == 0)
        return 0;
    const int div = c->m_field0 / c->m_field4;
    if (div <= 0)
        return 0;
    outCost = endurance - endurance % div;
    return endurance / div;
}