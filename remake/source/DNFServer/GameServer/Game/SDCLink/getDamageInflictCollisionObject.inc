float getDamageInflictCollisionObject(STComputeDamageVariable* v, void* collisionObject)
{
    float dmg = v->m_field000;
    if (v->m_char004 != 0)
    {
        if (v->m_damageType == 0 || v->m_damageType == 2)
            dmg = dmg + v->m_field02F;
        else if (v->m_damageType == 1)
            dmg = dmg + v->m_field033;
    }
    dmg = dmg * (v->m_f006 * kDamageRateMul01 + 1.0f);
    if (v->m_damageType == 0)
        dmg = dmg * (v->m_f037 * kDamageRateMul01 + 1.0f);
    else if (v->m_damageType == 1)
        dmg = dmg * (v->m_f03B * kDamageRateMul01 + 1.0f);
    if (v->m_damageType == 0)
    {
        float s2 = v->m_f03F;
        if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
            s2 = s2 * getAbilityConvertRateSDC(collisionObject, CONVERT_TABLE_3);
        dmg = dmg * (g_physicalDamageRateConst_ / s2 + 1.0f);
    }
    else if (v->m_damageType == 1)
    {
        float s2 = v->m_f043;
        if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
            s2 = s2 * getAbilityConvertRateSDC(collisionObject, CONVERT_TABLE_5);
        dmg = dmg * (g_magicalDamageRateConst_ / s2 + 1.0f);
    }
    dmg = dmg * getAbilityConvertRateSDC(collisionObject, CONVERT_TABLE_0);
    dmg = v->m_f047 * dmg + dmg;
    return dmg;
