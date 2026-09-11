// [b28d-L28 P317 收口 2026-09-09] 同上删 `~stAssaultChaosPointWeight(){}` ⇒
// p167 OUR calls 12 → 9 == ORIG 9 **CLEAN**（EXTRA `D1Ev`×2 + `_Unwind_Resume` 全消，
// 仅余 `C2Ev`/`C1Ev` 同址别名 P93 伪差）；gap 14 → **5**（quick/objdump 实测 135/130）。
// 残余 5 条（判 N）：① 两处 x87 比较 lowering `fucompp/fnstsw %ax/sahf` vs
// `fucomip %st(1),%st/fstp`（N85 族）；② 一处 ORIG `mov $imm,%eax; mov %eax,-槽`
// 常量重赋值（源码具名 float 重赋值，P244）+ `flds` 同槽两次。
float ServerParameterScript::getAssaultPriceChaospointWeight(int chaosPoint)
{
    // b28d-solo-ServerParameterScript: DIFF→修（P243 flds 序 + early-return 汇聚）
    // ORIG 0x08a8c668 逐段：① `flds w0; flds w1; flds w0; fsubrp` ⇒ 原码把 `+w0`
    // 写在**最左**（`w0 + (w1-w0)/(max-min)*(chaos-min)`），我方旧式 `... + w0` 使 4.4
    // 末尾重复 flds w0（多 1 条）；② 两处 clamp 在 ORIG 为 `mov 字段,%eax; jmp 尾中转`
    // （直接返回字段，不回写 weight 槽）⇒ 原码是 early-return 而非回写；③ x87 比较
    // fucompp/fnstsw/sahf vs 4.4 fucomip/fstp 属 N85 跨版本（P230 需 TU 旗标 -march=i486）。
    if (m_chaosWeight4e4.empty())
    {
        return 1.0f;
    }
    if (chaosPoint > 1000)
    {
        chaosPoint = 1000;
    }
    std::vector<stAssaultChaosPointWeight>::iterator it =
        m_chaosWeight4e4.begin();
    stAssaultChaosPointWeight w;
    for (; it != m_chaosWeight4e4.end(); it++)
    {
        if (chaosPoint >= it->m_min && chaosPoint <= it->m_max)
        {
            w = *it;
            break;
        }
    }
    if (w.m_w0 == 0.0f)
    {
        return 1.0f;
    }
    // [b28d-solo-C04] P0 槽往返：ORIG 0x08a8c786 `mov $0x3f800000,%eax;
    // mov %eax,-0xc(%ebp)`（守卫**之后**、计算**之前**）⇒ 原码有具名 float 局部
    // 带初值 `= 1.0f`，随后整式覆写。探针 /tmp/p1/probe7.cpp h1/h2 双形同产该对。
    float weight = 1.0f;
    weight = w.m_w0 + ((w.m_w1 - w.m_w0) /
                       (float)(w.m_max - w.m_min)) *
                          (float)(chaosPoint - w.m_min);
    if (weight < w.m_w0)
    {
        return w.m_w0;
    }
    if (weight > w.m_w1)
    {
        return w.m_w1;
    }
    return weight;
}