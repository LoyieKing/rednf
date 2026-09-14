// df_guild_r — MemPool 显式实例化点
// 模板完整定义在 MemPool.h（使用方 TU 隐式实例化自身池类型）。
#include <stdio.h>
#include <stdlib.h>

#include "MemPool.h"

// [df_guild_r 修复 2026-09-14] 下面的显式实例化属 df_game_r 口径，对 guild 既编译
// 不过、也不该链入：
//   1) CInGameAdvertisementManager.h 位于 Game/ 域，不在 guild 的 include 路径
//      （guild CMakeLists 只给 ServerCommon / Guild / packet include / shared）；
//   2) ORIG df_guild_r 全符号表中不存在任何 CAdvertisementExposeInfo /
//      MemPool<CAdvertisementExposeInfo> 符号（nm 匹配为空），而注释旧引用的
//      headOfFreeList_ 0x093feaec 超出本二进制装载范围（2254651B、装载上界约
//      0x0826e6bb），故该证据不属于 df_guild_r；
//   3) guild 无任何 TU 引用 CAdvertisementExposeInfo。
// 因此按工程口径对 guild 关掉（Game 侧构建不受影响，仍保留原实例化）。
#ifndef DNF_SVC_GUILD
#include "CInGameAdvertisementManager.h"

template class MemPool<CAdvertisementExposeInfo>;
#endif
