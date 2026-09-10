bool isValidAttackInfoElement(STComputeDamageVariable* v, int elem)
{
    if (elem == 4)
    {
        if (!(v->m_elemEnable[0] || v->m_elemEnable[1] ||
              v->m_elemEnable[2] || v->m_elemEnable[3]))
            return true;
        return false;
    }
    return v->m_elemEnable[elem] != 0;
