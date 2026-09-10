int getComputeDamageBasisLevelAttacker(STComputeDamageVariable* v, bool useAttackerLevel)
{
    if (useAttackerLevel)
    {
        if (_SDC::isObjectType(v->m_attackerObjType, 0x111))
            return (signed char)v->m_char1B4;
    }
    return (signed char)v->m_char1B3;
