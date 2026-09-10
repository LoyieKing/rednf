// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CWorldMap/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CWorldMap.cpp —— df_game_r 还原（OrigWorldMap16 批次，2026-08-29）。
// 符号经 ORIG nm/objdump 核实（df_game_r 直读，见函数旁注释）。
//
// 本 TU 独家提供（勿在他处重复定义）：
//   CWorldMap::get_index（ORIG W 0x83746ac）
//   CWorldMap::GetDungeonList（ORIG W 0x85beda4）
// 其余 CWorldMap 成员（C1/set_world_map/IsInHellDungeon/hasDeathTower/
// getDungeonMinLevel/hasDungeon）分属 CtorProducers_1.cpp / CDataManagerScripts.cpp。
// ============================================================================

#include "CWorldMap.h"

// ORIG W 0x83746ac _ZN9CWorldMap9get_indexEv（0xa 字节）：
//   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; mov (%eax),%eax; pop ebp; ret
// 非 const 成员（mangled 无 K），直返 m_index（this+0x0）。
// 消费方：CWorldMapList::insert_world_map（0x08364694 call 0x83746ac）。
#include "CWorldMap/get_index.inc"
}

// ORIG W 0x85beda4 _ZN9CWorldMap14GetDungeonListEv（0xb 字节）：
//   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; add $0x10,%eax; pop ebp; ret
// 非 const 成员，直返 &m_dungeonList（this+0x10）。
// 消费方：CParty::make_blood_dungeon_info（0x085b3e14 区）对返回值 begin()/end()
// 遍历（Ghidra：GetDungeonList(...); vector<int>::begin();）。
#include "CWorldMap/GetDungeonList.inc"
}
