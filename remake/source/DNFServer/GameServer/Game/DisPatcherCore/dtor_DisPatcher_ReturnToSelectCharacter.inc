DisPatcher_ReturnToSelectCharacter::~DisPatcher_ReturnToSelectCharacter() {}
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 77 / ours 77 insn 全等（AE 口径，重定位归一）。
