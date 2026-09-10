// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BattleMonsterTailC3A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// BattleMonsterTailC3A.cpp — BattleMonsterDepsC2 剩余依赖第一组（5 符号，ORIG nm 全部 T 实证）。
// 依据 docs/class_func_reports 逐函数汇编+反编译 C 转写；字段化布局（无裸偏移/asm 桥）。
//
//   _ZN13CBattle_Field17ConsistTournamentEPK4CMapRSt4listI11_mapMonsterSaIS4_EERSt3mapIs...
//       CBattle_Field::ConsistTournament @ 0x083035c8
//   _ZN13CBattle_Field27checkClearedNamedMonsterMapEhi
//       CBattle_Field::checkClearedNamedMonsterMap @ 0x08300c76
//   _ZN12CDataManager28get_hellparty_groupOrder_mapEi
//       CDataManager::get_hellparty_groupOrder_map @ 0x0836379a
//   _ZN13CBattle_Field22add_hell_party_monsterER11map_monsterRiS2_PSt3mapIt21STHellPartyGroupOrder...
//       CBattle_Field::add_hell_party_monster @ 0x083071ba
//   _ZN13CBattle_Field20getAddChampionMobCntEv
//       CBattle_Field::getAddChampionMobCnt @ 0x083010ee
//
// 依赖（均由已登记 TU 提供）：MakeBasicRoundInfo/ChoiceFirstGroup/ConsistTournamentRound
// （TournamentTailC3C.cpp）、TournamentInfo::operator<（CBattle_Field.cpp）、
// map_monster 生命周期/isNamedMonster 与 MapInfo::Add_Mob（WarField.cpp/MapInfo.cpp）、
// CDungeon::get_standard_level、CDataManager::find_hellparty_group/get_hellparty_script_values、
// Get_instance_groupOrder_map_pool（定义见 TournamentTailC3C.cpp）。

#include <list>
// ============================================================================
// b28d-52 / SweepBattleMonsterC3A 判词表（fresh check_tu_routed，四数
// 100/155/0/4/57 → 101/156/1/3/55；gap = ORIG−OUR）：
//   get_hellparty_groupOrder_map 0x0836379a —— gap 0（identical/ae）
//   getAddChampionMobCnt        0x083010ee —— gap 0（identical/ae）
//   ConsistTournament           0x083035c8 —— gap 0，NEAR：P264（.o 数据立即数/重定位
//     显示差）+ N82 槽号；calls 30=30、callee 差集仅 C1/C2 同址别名（P93）。
//   add_hell_party_monster      0x083071ba —— gap −1，DIFF：calls 41=41、差集全为
//     C1/C2·D1/D2 同址别名；残余 1 条 = script_values 段 4.4 把返回值 `mov %eax,%edx`
//     中转（ORIG 4.1 用 %edx 装 this）⇒ N7 寄存器角色封顶。
//   checkClearedNamedMonsterMap 0x08300c76 —— gap +6，DIFF：calls 7=7（Main 代落
//     isNamedMonster() throw() 后 _Unwind_Resume 消失），差集仅 C1/C2 别名；残余
//     全为 4.1 代码生成风格（esi 路径标志 + 实参求值槽）+ char m_visited 判零 4.4
//     sete（P73 已知，字段 bool 化会破坏 set_visited_map 的 AE 写入形，见
//     BattleMonsterDepsC2.cpp:1083 判词）⇒ N 封顶。
// ============================================================================
#include <map>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "DataManagerInitDepends.h"   // stHellParty_t/stHellPartyMonsterGroupList（须在 CDataManager.h 之后）
#include "HellPartyGroupOrder.h"

// ==================================================================
// CBattle_Field::ConsistTournament @ 0x083035c8
// 报告：docs/class_func_reports/CBattle_Field/ConsistTournament.md
// [SweepBattleMonsterC3A] DIFF→NEAR（gap 209/209=0；calls 30=30，差集仅 C1/C2 同址
// 别名）：①P227 错误分支改 cMyTrace 全表达式临时链（ORIG 0x083036e9 在 ctor 前
// `call get_member_count; mov %eax,%ebx` 跨 ctor 保活）；②P198① MakeBasicRoundInfo
// 结果用具名 bool 局部（ORIG 0x0830377c `mov %al,-0x9; movzbl -0x9,%eax; xor $1`）。
// 残差 = P264（.o 数据立即数/重定位显示差）+ N82 槽号。
// ==================================================================
#include "BattleMonsterTailC3A/ConsistTournament.inc"
}

// ==================================================================
// CBattle_Field::checkClearedNamedMonsterMap @ 0x08300c76
// 报告：docs/class_func_reports/CBattle_Field/checkClearedNamedMonsterMap.md
// [SweepBattleMonsterC3A] 符号线索闭环：ORIG 0x08300cee/0x08300d15 调 **const** 版
// `_ZNKSt6vectorI20STNamedMonsterMapPosSaIS0_EEixEj` @0x0830fec0（全库无**非** const
// 版定义；直接写 `[idx]` 会误选非 const 版 ⇒ 我方缺失该 ORIG 符号）⇒ 源码用 const
// 引用视图 cast（P323），
// p167 MISSING `…ixEj`×2 / EXTRA 非 const×2 已归零。Main 代落 `isNamedMonster()`
// throw()（WarField.h/MapInfo.cpp）后 `_Unwind_Resume` 消失、calls 9→7=ORIG，
// gap 81/75=+6：残差全为 4.1 代码生成风格（esi 路径标志 `mov $0/$1,%esi`+push/pop
// esi、GetMapInfoFromPos 实参求值槽 -0x10/-0xc）+ char m_visited 判零 4.4 sete（P73）
// ⇒ N 封顶（语义等价：p167 calls 7=7、差集仅 C1/C2 别名）。
// ==================================================================
#include "BattleMonsterTailC3A/checkClearedNamedMonsterMap.inc"
}

// ==================================================================
// CDataManager::get_hellparty_groupOrder_map @ 0x0836379a
// 报告：docs/class_func_reports/CDataManager/get_hellparty_groupOrder_map.md
// 返回 map<int, HellPartyGroupOrderMap> 中 key 对应内层 map 地址；未命中返回 NULL。
// ==================================================================
#include "BattleMonsterTailC3A/get_hellparty_groupOrder_map.inc"
}

// ==================================================================
// CBattle_Field::add_hell_party_monster @ 0x083071ba
// 报告：docs/class_func_reports/CBattle_Field/add_hell_party_monster.md
// [SweepBattleMonsterC3A] 符号线索闭环：ORIG 增量段调 **后置** `…ppEi` @0x08311c40
// （P28 增量侧推论）⇒ 两处 `rit++`；我方 .o 现已发射 ppEi（前置 ppEv 消失）。
// 其余实修（全部有 ORIG 地址直证）：入口 4 具名局部 0/0/1/0（0x083071c5..da）；
// rit=begin() 提前到 totalRate 之前（0x08307239）；totalRate 为 int 槽（0x0830727f
// `movzwl %ax,%eax; mov %eax,-0x18`）；i=0 在 find 之前（0x0830731e）；m_hellPartyGroupIdx
// 与 oit->first 同型化（0x083073ca 单段）；m_roleType 反极性 if/else（0x0830726c jne）；
// m_field11b 双字节视图 + Add_Mob 匿名临时（0x083074b4 单次 C2ERKS_）。
// 残差 gap −1 = script_values 段 4.4 `mov %eax,%edx` 中转（N7）。
// ==================================================================
#include "BattleMonsterTailC3A/add_hell_party_monster.inc"
}

// ==================================================================
// CBattle_Field::getAddChampionMobCnt @ 0x083010ee
// 报告：docs/class_func_reports/CBattle_Field/getAddChampionMobCnt.md
// ==================================================================
#include "BattleMonsterTailC3A/getAddChampionMobCnt.inc"
}
