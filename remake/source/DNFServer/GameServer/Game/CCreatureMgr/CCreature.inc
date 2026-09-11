// ORIG CCreature @ 0x833705e：基类构造（+0 ← 0x8c2f4d8）→ vptr 覆盖
// （+0 ← 0x8c2f4c0）→ CStomachC1（+0x34=当前秒、+0x38=100.0f）→
// COverSkillTimeC1（+0x48=0）→ Reset()。镜像无虚表，m_vptr 常量模拟
// vtable 指针；memset 在成员自动构造之后执行会覆盖子对象构造结果，
// 故补回 CStomach 初始值（Reset 随后把 m_value 覆盖为 0.0f，与 ORIG 一致）。
// 镜像无虚表：以 placement new 显式调用 CCreatureItemC1 模拟 ORIG 基类构造
// （写 +0x00=0x8c2f4d8，随后被 vptr 覆盖），与 ORIG call 序列一致；
// 余下字段不初始化（同 ORIG，由 Reset 覆盖）。
// [b28d-23 N·N83] 镜像无虚表：ORIG 21 条（直 call CCreatureItemC1）vs 4.4 55 条（placement new _ZnwjPv + EH cleanup）⇒ N。ORIG 0x0833705e
// [b28d-solo-CCreatureMgr N] N83：镜像类 placement new（_ZnwjPv + EH cleanup，55 条）vs ORIG 直 call 基类 C1（21 条）；ORIG 0x833705e
// [b28d-solo-CCreatureMgr R2上报] 唯一出路 = 头改动（Main 代落）：CCreatureItem 多态化 +
//   CCreature : public CCreatureItem。ORIG 0x833705e 21 条 = call CCreatureItem::C1 →
//   编译器自动写 vptr（_ZTV+8）→ CStomach::C1(+0x34) → COverSkillTime::C1(+0x50) → Reset；
//   我方 p167 EXTRA _ZnwjPv/_ZdlPvS_/_Unwind_Resume/_ZN11CSystemTime9getCurSecEv。
// [b28d-solo-CCreatureMgr AE·检查点14 已落] Main 落头改动后 21=21（compare_tu AE）：
//   我方 `call CCreatureItem::C2Ev` 对 ORIG `C1Ev` = P93 同址别名（0x8340c82）豁免；
//   vptr 写为 `movl $8,(%eax)+R_386_32 _ZTV13CCreature`（AE 伪代码化）。
CCreature::CCreature()
{
    // [b28d-Main·检查点14 真多态化] 基类 ctor 由编译器隐式调用（ORIG 0x833705e
    //   `call CCreatureItem::C1Ev` + `movl $0x8c2f4c0,(%eax)` 自动复现）；
    //   原 `new (this) CCreatureItem` + 手工 m_vptr 写已删（消 N83 placement-new）。
    // [b28d-Main·检查点14] 删除冗余成员初始化：ORIG ctor 体仅 Reset()，
    //   +0x34/+0x38/+0x50 的初始化由 CStomach::C1Ev/COverSkillTime::C1Ev 完成
    //   （本仓库两成员 ctor 已同形）⇒ 复现 ORIG 21 条。
    Reset();
}
