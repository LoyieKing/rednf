// CMissionRank.cpp — CMission_rank 迁移实现（原 GameStubs.cpp 定义）。
// ORIG 实证（nm -S + vtable dump）：_ZTV13CMission_rank @0x08cc3f80 sz 0x40，
// 其 verifyMission 槽（第 6 槽）= __cxa_pure_virtual @0x08724fe0 ⇒ ORIG 的
// CMission_rank 未 override 基类纯虚 verifyMission（故无 _ZNK13CMission_rank13
// verifyMissionERK11MissionInfo 符号）。原自造占位定义（return false）已删除，
// 类保持抽象、该槽回到 __cxa_pure_virtual，与 ORIG 一致。
#include "CMission.h"
