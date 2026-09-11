// ============================================================================
// 4) _Z16getDamageInflictP23STComputeDamageVariablePv @0x08095193 (0x8d)
//    按 v+0x1BA（m_field1BA）对象类型三分派：
//      isObjectType(0x21) → PassiveObject(v,obj)；isObjectType(0x81) → Creature(v)；
//      否则 CollisionObject(v,obj)。
// ============================================================================
float getDamageInflict(STComputeDamageVariable* v, void* defender)
{
    if (_SDC::isObjectType(v->m_field1BA, 0x21))
        return getDamageInflictPassiveObject(v, defender);
    if (_SDC::isObjectType(v->m_field1BA, 0x81))
        return getDamageInflictCreature(v);
    return getDamageInflictCollisionObject(v, defender);
}