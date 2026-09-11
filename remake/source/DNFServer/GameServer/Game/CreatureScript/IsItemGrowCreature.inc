// ORIG 0x833f320：if (m_field5c==-1 && m_field60==-1) return 0 else return m_field4c。
bool CCreatureScript::IsItemGrowCreature()
{
    if (m_field5c == -1 && m_field60 == -1)
    {
        return false;
    }
    return m_field4c;
}