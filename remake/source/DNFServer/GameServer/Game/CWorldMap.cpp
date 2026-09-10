// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CWorldMap/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CWorldMap.h"

// ORIG W 0x83746ac _ZN9CWorldMap9get_indexEv（0xa 字节）：
//   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; mov (%eax),%eax; pop ebp; ret
// 非 const 成员（mangled 无 K），直返 m_index（this+0x0）。
// 消费方：CWorldMapList::insert_world_map（0x08364694 call 0x83746ac）。
#include "CWorldMap/get_index.inc"
{
    return m_index;
}

// ORIG W 0x85beda4 _ZN9CWorldMap14GetDungeonListEv（0xb 字节）：
//   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; add $0x10,%eax; pop ebp; ret
// 非 const 成员，直返 &m_dungeonList（this+0x10）。
// 消费方：CParty::make_blood_dungeon_info（0x085b3e14 区）对返回值 begin()/end()
// 遍历（Ghidra：GetDungeonList(...); vector<int>::begin();）。
#include "CWorldMap/GetDungeonList.inc"
{
    return m_dungeonList;
}
