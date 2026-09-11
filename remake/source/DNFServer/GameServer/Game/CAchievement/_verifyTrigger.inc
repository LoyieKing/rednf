// [b28d-solo-CAchievement AE] 修法：ORIG 0x0828b81e 为 `mov 0xc(%ebp),%eax` +
// 三次 `cmp $imm,%eax`
// （寄存器链，P286/P251/P261）⇒ 源形是 switch（if 链在 4.4 恒 mem 直比，P209）。
// ORIG cmp 顺序 198b→19c4→1983 且两个 level 块 tail-merge 到同一地址 +0x6c。
// level 极性按 ORIG `cmp $0x45; setle %al; test; je <mov $1>` ⇒
// 源形 `if (level <= 0x45) return 0; return 1;`（P56/P292）。
bool CAchievement::_verifyTrigger(unsigned int id, unsigned short t0,
