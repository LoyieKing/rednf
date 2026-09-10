bool CCreatureScript::IsItemGrowCreature()
{
    if (m_field5c == -1 && m_field60 == -1)
    {
        return false;
    }
    return m_field4c;
