// ORIG _ZN13user_creature9CCreature13IsDieCreatureEv T 0x8338478：
// GetStomachValue(m_state, m_isGrow) < 1.0f（ORIG fucompp+test0x45 sete=value<1）。
// [b28d-23 N·N85/P230] x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp/seta）；-march=i486 可消但本 TU 未路由且禁改路由表 ⇒ N。ORIG 0x08338478
// [b28d-solo-CCreatureMgr AE] P252 具名 float 局部复现 fstps/flds 槽往返 + TU 路由 -march=i486 消 x87 lowering；ORIG 0x8338478
bool CCreature::IsDieCreature()
{
    // ORIG 0x833a660 sub $0x28：float 比较物化 spill 槽 -0xc(fstps/flds)+slack。
    // 4.4 栈顶保留不占槽 → 帧差 0x10；int[4] 16B 对齐恰好补齐（dead pad）。
    // [b28-BM7 判词] 残余 = N85：ORIG `fstps/flds; fucompp; fnstsw %ax;
    // test $0x45,%ah; sete %al` vs 4.4 `fucomip %st(1),%st; fstp %st(0); seta`
    // （x87 比较两向+槽往返跨版本固有差）。语义（value<1 ⇒ die）全等。
    // [b28d-solo-CCreatureMgr P252] ORIG `fstps -0xc(%ebp); flds -0xc(%ebp)`
    // ⇒ 原码有具名 float 局部（N85 突破口）；直返表达式会省掉该槽往返对。
    float stomachValue =
        m_stomach.GetStomachValue((CREATURE_STATE)m_state, m_isGrow);
    return stomachValue < 1.0f;
}
