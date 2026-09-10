float getDamageInflict(STComputeDamageVariable* v, void* defender)
{
    if (_SDC::isObjectType(v->m_field1BA, 0x21))
        return getDamageInflictPassiveObject(v, defender);
    if (_SDC::isObjectType(v->m_field1BA, 0x81))
        return getDamageInflictCreature(v);
    return getDamageInflictCollisionObject(v, defender);
