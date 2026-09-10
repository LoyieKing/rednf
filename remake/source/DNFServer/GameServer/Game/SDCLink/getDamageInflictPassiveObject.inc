float getDamageInflictPassiveObject(STComputeDamageVariable* v, void* passiveObject)
{
    return (v->m_char1B5 != 0 && v->m_char1B6 != 0)
               ? getDamageInflictCreature(v)
               : getDamageInflictCollisionObject(v, passiveObject);
