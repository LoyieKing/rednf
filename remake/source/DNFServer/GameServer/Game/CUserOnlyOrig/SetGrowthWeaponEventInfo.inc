// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08691bae：gap=1。修法（本轮）：memcpy 逐字段
// 改具名载荷视图直读直存（P235/P250，117→80 条）；视图 short 与目标 m_field10/12 同型
// 消 movzwl 中转（P211 档③）；`&&` 拆嵌套 if 复现双 setcc 段（P225 推广）。残余
// ORIG `movzbl field; xor $1,%eax; test %al,%al; je` vs 4.4 `cmp $1,%al; je`：
// 探针 pxor.cpp a1..a4 / pxor2.cpp a5..a8 实证——bool 字段+`!field` 才产 xor 形，
// 但字段 bool 化会使本函数写入侧（bool 源→bool 目标）由 RHS-first 翻 LHS-first
// （探针 setB vs setU），破坏已匹配的 `movzbl; mov %dl,disp` 形 ⇒ P168a④/N96+N18a 封顶。
void CUser::SetGrowthWeaponEventInfo(const SIG_GROWTH_WEAPON_EVENT& sig)
