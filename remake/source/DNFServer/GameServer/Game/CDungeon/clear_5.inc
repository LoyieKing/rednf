// ORIG W 0x088f2c64（报告缺失兜底：nm 定址+objdump 切片；清单给
// docs/class_func_reports/CCharacter/clear.md 系撞名报告，签名为
// _ZN10CCharacter5clearEv，已按 §6.3 弃用）：clear 整体 memset(this,0,0x10)。
// [b22判词·跨版本] 残余 1 序差：4.4 填 memset 出参槽先于取 this，4.1 先生成
// `mov 0x8(%ebp),%eax`（指令集相同仅两 mov 对调，三变体实测 4.4 无法复现前序）；
// call 目标 reloc 即 memset（§6.3 占位假阳性）。语义等价。
void STMonsterTypeSpawnProb::clear()
{
    // P68 正向：ORIG 0x88f2c6a 实参序「this 先物化→0x10→0x0」＝ 带 cast 形。
    memset((char*)this, 0, sizeof(STMonsterTypeSpawnProb));
}