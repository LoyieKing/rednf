int CCreatureScript::GetItemEvolutionCreatureId(int creatureId)
{
    // ORIG 0x833cb15：ne 的 this=-0xc（end 临时槽）、arg=-0x10（find 结果 it
    // 槽）⇒ 原码比较形为 end != it（P220 极性翻正消实参装载序差）。
    std::set<int>::const_iterator it = m_set34.find(creatureId);
    if (m_set34.end() != it)
    {
        return creatureId;
    }
    return 0;
