void omll_dump_core_file() { std::abort(); }

// 15/10/12/13/11/8/4/7/16/23/24/25/31 → signal_handler（13 → SIG_IGN），
// 任一注册失败即返回 0，全部成功返回 1。
// [b28d-28] DIFF | ORIG 0x0829f4c5 | P167 CLEAR（13/13）；N82 槽/寄存器分配差。
// [b28d-solo-GameMain] AE | 131=131 | p167 13/13 CLEAR | 修法：bool ok 槽形（前 3 次
// 直测 + 后 10 次 `mov %al,-0x9` 槽，P198① 预清）+ regist_signal 返回类型 int→bool
// （P248 档①：调用点 `xor $1,%eax` 复现；返回类型不入 mangled）。
