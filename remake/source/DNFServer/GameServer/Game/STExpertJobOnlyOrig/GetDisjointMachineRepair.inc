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
