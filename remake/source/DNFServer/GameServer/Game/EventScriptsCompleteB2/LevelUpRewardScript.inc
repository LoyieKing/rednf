// ORIG W 0x089861da：基类 → string+0xc → map+0x10 → mail+0x28。
// +0x08 字节故意不初始化（is_recv_check_item 读此字节；[recv check db table]
// 未出现时保持未定值，忠实保留）。
LevelUpRewardScript::LevelUpRewardScript() {}
