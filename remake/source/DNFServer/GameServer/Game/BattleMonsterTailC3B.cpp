// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BattleMonsterTailC3B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// BattleMonsterTailC3B.cpp — CBattle_Field 血战收尾/访问图查询 + CDungeon
// ChooseRandomMap 剩余链接依赖（独占 TU，宿主登记 CMake；不改 GameStubs，
// 不重复 CBattle_Field_unresolved.cpp / BattleMonsterTailC3A.cpp /
// BattleMonsterDepsC2.cpp 既有定义）。
//
// 恢复符号（ORIG nm 全部实证，df_game_r）：
//   _ZN13CBattle_Field17add_blood_monsterER11map_monsterRiS2_St6vectorI19STAssignBloodSystemSaIS4_EES3_I22STAssignBloodPhaseTimeSaIS7_EE
//                                                        T 0x08304804
//   _ZN13CBattle_Field14is_visited_mapEii                T 0x08307c7a
//   _ZN13CBattle_Field19is_visited_map_gotoEii           T 0x08307cbe
//   _ZN13CBattle_Field18on_not_visited_mapEiiRb          T 0x08307d0c
//   _ZNK8CDungeon15ChooseRandomMapEiii                   T 0x0834bf56
//
// 报告依据（编辑前已完整读 ##汇编 + ##反编译 C 全文）：
//   docs/class_func_reports/CBattle_Field/{add_blood_monster,is_visited_map,
//     is_visited_map_goto,on_not_visited_map}.md
//   docs/class_func_reports/CDungeon/ChooseRandomMap.md
// 内联弱符号调用点（ORIG W，本 TU 仅调用不定义；定义点由宿主统一归属）：
//   CBattle_Field::GetCurrentMapInfo()            W 0x0822cbcc（getCurPosXY→GetMapInfoFromPos(x,y,false)）
//   CBattle_Field::GetMapInfoFromPos(int,int,bool) W 0x0822cc48（m_mapInfo(+0x78) find/insert）
//   CBattle_Field::swap_curpos_xy(int,int)         T 0x0830878e
//   CDataManager::find_map(int) const              T 0x0835fa14 / find_monster(int) const T 0x0835fd84
//   MapInfo::Add_Mob(map_monster)                  W 0x08151612（按值）
//   CBattle_Field::CBloodRound::setBloodLastSmallRoundOrder(int) W 0x0830eb96
//   CMap::get_map_type() const W 0x0830e5c0 / CMap::get_index() const W 0x08134776
//   get_rand_int(int) 0x086b1b87 / rand() @plt / LogManager::logFormat 0x08ad3c0a
// 常量：add_blood_monster 中 flds 0x8c23d70 = float 1000.0（objdump 实测，
//   mob.m_bloodPhaseTime(+0x33c) = (short)(1000.0f * phaseTime.m_time)）。
// ============================================================================

#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h      // CBattle_Field + WarField.h(MapInfo/map_monster) +
                                // CMap.h(STAssignBloodSystem/STAssignBloodPhaseTime/CMap) +
                                // CDungeon.h(CDungeon/m_index/m_dummyMap*/m_map1a4)
#include "CDataManager.h"       // G_CDataManager()/find_map/find_monster
#include "GameRand.h"           // get_rand_int
#include "LogManager.h"         // LogManager::logFormat

// ---------------------------------------------------------------------------
// CBattle_Field::is_visited_map — ORIG 0x08307c7a
// GetMapInfoFromPos(x,y,false) 后取 MapInfo+0x01 访问标记。
// ---------------------------------------------------------------------------
#include "BattleMonsterTailC3B/is_visited_map.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::is_visited_map_goto — ORIG 0x08307cbe
// 访问标记(+0x01)或 goto 标记(+0x02)任一置位即为真。
// ---------------------------------------------------------------------------
#include "BattleMonsterTailC3B/is_visited_map_goto.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::on_not_visited_map — ORIG 0x08307d0c
// 置访问标记 → swap_curpos_xy 换位 → 以新当前图索引自 CDataManager 取图并更新。
// ---------------------------------------------------------------------------
#include "BattleMonsterTailC3B/on_not_visited_map.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::add_blood_monster — ORIG 0x08304804
// 血战按阶段时间表逐阶段生成怪物：
//   阶段 m_phaseIdx==-1 时随机取系统序号并记录到 m_bloodPhaseOrder(+0x258)；
//   每阶段先 push false 到 m_bloodPhaseClear(+0x264)；
//   命中随机疯狂阶段(crazyRound==阶段键)时掷 100 决定疯狂怪物类型并写
//   m_bloodSpawnCntCur(+0x2a4)/m_bloodCrazyRound(+0x2a8)/m_bloodCrazyType(+0x2aa)；
//   内层按累计数量生成 map_monster 并 Add_Mob 到当前图，mobId 在数据管理器
//   找不到时整体返回 false；全部阶段完成后写 lastSmallRoundOrder。
// 局部布局按 ORIG 栈槽逐一对位（含两个从不读取的死局部 -0x42/-0x40）。
// ---------------------------------------------------------------------------
// [SweepBattleMonsterC3B 2026-09-09] 已修至 AE（orig=384/ours=384，check_tu_routed
// fresh .o；p167 差集仅 C1/C2·D1/D2 同址别名=P93 伪差）。本轮 6 处修正（均符号级直证）：
//  ① `(*it).`→`it->`：ORIG 循环体 6 处 `call ptEv`（operator->() 0x083118bc；我方原缺
//     该 W 符号）——P167 新判据 it箭头 vs 星点；
//  ② `it++`→`++it`：ORIG 增量段 `call ppEv`（0x083118e0；我方原发 ppEi=后置）——P28；
//     且 smallRoundOrder++ 并入 for 增量逗号（ORIG 顺序 0x08304d5d ppEv → 0x08304d6b addw）；
//  ③ +0x11/+0x12 逐字节写（m_field11b 双视角）并补 +0x34 清零：ORIG 0x08304b99/ba0/b5；
//  ④ find_monster 结果落具名槽 -0x18（0x08304d4d `mov %eax,-0x18(%ebp)`）——P0；
//  ⑤ 三目→if/else：ORIG `cmp $1; sete %al; test %al,%al; je` + 两分支各自写 -0x24
//     （0x083048c5..0x083048ea；三目在 4.4 产 jne+汇合点单次存储）——探针 /tmp/probe_crazy.cpp fB；
//  ⑥ `it->m_time * 1000.0f` 书写序（P243：ORIG 先 flds 0xc(%eax) 后 flds 常量 0x8c23d70）。
// 残余仅 `flds 常量` 的 .o 重定位显示差（AE 归一后相等）。
#include "BattleMonsterTailC3B/add_blood_monster.inc"
}

// ---------------------------------------------------------------------------
// CDungeon::ChooseRandomMap — ORIG 0x0834bf56（const）
// 按 map_type 选容器：1 → boss_maplist_[dir](+0x324)、2 → dummy_maplist_[dir]
// （+0x4a4）、其余 → maplist_[dir](+0x1a4)；entrance==0 全表随机游走取
// second->get_index()，否则 equal_range(entrance)/count(entrance) 后同型游走。
// 各失败路径按 ORIG 行号/文案记日志（map_type 实参始终传形参本尊）。
// 容器映射与 InsertMap（BattleDungeonMapA2）的 get_map_type 分支一致。
// ---------------------------------------------------------------------------
#include "BattleMonsterTailC3B/ChooseRandomMap.inc"
}
