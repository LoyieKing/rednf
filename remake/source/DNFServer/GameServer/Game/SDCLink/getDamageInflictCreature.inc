// _Z24getDamageInflictCreatureP23STComputeDamageVariable @0x08095508 (0xd9)：
// m_char1B5 门控；dmg=m_field000*(rate(m_char1B7,m_char1B8,(int)m_d1AA))*m_f02B
// *FromModule(m_objTypeByte)；objTypeByte==9 再 *FromIndex(m_char1B9)。
// [b28d-lane15 判词·N（N85）] → [C03CryptoInit68 修·2026-09-10] 两处已翻正：
//  ① 乘法书写序（P243 变体）：写 `rate(...) * base` ⇒ 4.4 发
//     `call; fmuls -0x14(%ebp); fstps -0xc(%ebp)`，**累加器槽 -0xc 与 ORIG 一致**
//     （旧 `base * rate()` 产 flds+fmulp 三条且累加器落 -0x10）。
//  ② 具名 float 副本（P252/P0）：ORIG `flds (%eax); fstps -0x24` 是具名局部
//     ⇒ 补 `float base = v->m_field000;` 并把 dmg 拆为独立变量后，gap 65→**64=ORIG**。
// 残余（跨版本族，语义等价）：ORIG x87 拷贝 `flds/fstps` vs 4.4 `mov/mov`（N85①）；
// base/lv 槽位比 ORIG 浅 0x10（N126 版本首槽基址常数 + N82 槽复用，实测非声明序可翻）；
// `mov -0x20(%ebp),%ecx` 装载时刻（P75）。按 §6 记 N 封顶。
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
}