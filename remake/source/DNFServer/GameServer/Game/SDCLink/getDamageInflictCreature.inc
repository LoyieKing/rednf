float getDamageInflictCreature(STComputeDamageVariable* v)
{
    if (v->m_char1B5 == 0)
        return 0.0f;
    float base = v->m_field000;
    int lv = static_cast<int>(v->m_d1AA);   // fldcw 截断模式
    float dmg = getDamageRateCreature(
                    static_cast<int>(static_cast<signed char>(v->m_char1B7)),
                    static_cast<int>(static_cast<signed char>(v->m_char1B8)), lv) * base;
    dmg = dmg * v->m_f02B;
    dmg = dmg * getCreatureDamageRateFromModule(
                     static_cast<ENUM_MODULE_TYPE>(static_cast<signed char>(v->m_objTypeByte)));
    if (static_cast<signed char>(v->m_objTypeByte) == 9)
        dmg = dmg * getCreatureDamageRateFromIndex(
                         static_cast<int>(static_cast<signed char>(v->m_char1B9)));
    return dmg;
