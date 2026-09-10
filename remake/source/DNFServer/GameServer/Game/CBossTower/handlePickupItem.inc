int CBossTower::handlePickupItem(CUser* user, int a, bool b, char c)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
// [P8S2 2026-09-11 负结论·P139] ORIG prett=0x4d `virtual bool WongWork::CBossTower::handlePickupItem(CUser*, int, bool, char)`（0x08b701a0，REF 见 ORIG 0x08143dec `movl $0x8b701a0,0x4(%esp)`）VS 我方手写串保留：CBossTower.h:59 声明为 `virtual int` ⇒ 换 __PRETTY_FUNCTION__ 会发射 `virtual int …`（size 不符）；修法=CBossTower.h 返回类型 int→bool（mangled 不变），属跨 TU 头改动 ⇒ 只报告不改（本泳道无 .h 写权）。
