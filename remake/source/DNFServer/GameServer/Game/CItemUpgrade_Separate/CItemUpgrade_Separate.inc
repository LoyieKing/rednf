// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN8WongWork21CItemUpgrade_SeparateC2Ev`（P327）。全限定定义可写在
// 命名空间块之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ORIG T 0x0811dfc0：基类 vptr → 本类 vptr → 表构造 → +0x190=100000 →
// new CMTRand(time) 存 +0x18c → 100 次 randInt(m_randRange) 热身。
// [K2F-1 负结论·N93 槽向] 残余=两个 4B 槽互换：ORIG seed 临时 -0x24（深）/range
// -0x20（浅）；4.4 匿名临时恒 -0x20（浅）/range -0x24（深）。两探针实测：
//   ① `unsigned long seed = ...; unsigned long range; ...new CMTRand(seed);`
//      → 槽仍 -0x20/-0x24（未换向），且 4.4 把 `lea -0x20(%ebp)` 推迟到调用点
//      重物化（ORIG 0x0811e007 `lea -0x24(%ebp),%esi` 在 `movl $0x9c8` 之前）
//      ⇒ 助记符序变，TU near 2→1 / diff 6→7（劣化）。
//   ② 交换两句声明（range 在前、seed 在后）：节点槽位翻正（seed -0x24/range -0x20）
//      但 lea 仍推迟 ⇒ 同样 near 1/diff 7。两形均回滚。
//   ⇒ 4.1「先声明深」vs 4.4 延迟取址 + 匿名临时浅槽，属 N93/N82 跨版本槽向，封顶。
WongWork::CItemUpgrade_Separate::CItemUpgrade_Separate()
