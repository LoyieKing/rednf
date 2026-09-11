// _Z31getDamageInflictCollisionObjectP23STComputeDamageVariablePv @0x08095275 (0x1bb)：
// 逐指令转写。0x8b12f54=0.01f；0x93f5d10/0x93f5d28=Damage 常量；
// getAbilityConvertRateSDC 表类型 3/5/0。
// [b28d-lane15 判词·N（N85②）] 残差仅 2 处除法：ORIG `flds 全局; flds -0x8;
// fdivp %st,%st(1)` 三步 vs 4.4 `flds 全局; fdivs -0x8` 两步（除操作数在内存时
// 4.4 恒折 fdivs）。其余 diff 行全为重定位地址伪差（0x8b12f54 vs .o 未解析 0x28）。
// 语义等价，封顶。
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
    dmg = dmg * (v->m_f006 * 0.01f + 1.0f);
    if (v->m_damageType == 0)
        dmg = dmg * (v->m_f037 * 0.01f + 1.0f);
    else if (v->m_damageType == 1)
        dmg = dmg * (v->m_f03B * 0.01f + 1.0f);
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
}