// _Z29getDamageInflictPassiveObjectP23STComputeDamageVariablePv @0x08095220 (0x55)：
// m_char1B5 && m_char1B6 → Creature(v)；否则 CollisionObject(v, passiveObject)。
// [b28c-lane24] 四形（双 return/if-else/if-else+init/P210 前置定义）帧恒 0x38 vs
// ORIG 0x28、尾物化槽 -0x1c vs -0xc ⇒ 曾判 N（跨版本帧策略 + 尾物化槽）。
// [b28d-lane15 翻案] ORIG 实为**三元**：两臂 fstps -0xc → 汇合 +0x4d
// `mov %eax,-0xc; flds -0xc`（float 位模式过 eax），帧 0x28、无具名 float 局部。
// 改三元后 fresh .o 判 identical/ae（SDCLink near 2→1）。P243 同族。
float getDamageInflictPassiveObject(STComputeDamageVariable* v, void* passiveObject)
{
    return (v->m_char1B5 != 0 && v->m_char1B6 != 0)
               ? getDamageInflictCreature(v)
               : getDamageInflictCollisionObject(v, passiveObject);
}