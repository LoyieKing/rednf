DisPatcher_MoveMap::~DisPatcher_MoveMap() {}
// [b28c-L19 属主修复] 参数名曾被注释但函数体用 &msg ⇒ TU 编译失败，恢复具名。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 71 / ours 71 insn 全等（AE 口径，重定位归一）。
