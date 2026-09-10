DisPatcher_ChangeSkill::~DisPatcher_ChangeSkill() {}
// ORIG 0x081c0196 T _ZN22DisPatcher_ChangeSkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_ChangeSkill/check_error.md 逐块映射）：技能树索引
// [b28d-29] N82/N18b：槽号平移 -0x10->-0x14 + 比较方向折痕（4.4 cmpl mem 直比 vs 4.1 两段），语义集合全等  (ORIG 0x081c0196 insn 31->39)
// ∈{-1,0} 时 msg+0xd==1 → 0x13；其它索引时 msg+0xd!=1 → 0x13；否则 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 18（ORIG 30 / ours 34），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
