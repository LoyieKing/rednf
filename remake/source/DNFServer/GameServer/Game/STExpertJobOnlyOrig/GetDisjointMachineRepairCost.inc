int STExpertJobScript::GetDisjointMachineRepairCost(int grade, int count)
{
    const stEnduranceRepairCost* c = _GetEnduranceRepairCost(
        static_cast<unsigned int>(grade));
    if (c == 0)
        return 0;
    return c->m_field0 - (c->m_field0 * count) / c->m_field4;
