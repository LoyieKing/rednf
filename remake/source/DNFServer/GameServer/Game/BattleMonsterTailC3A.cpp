// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BattleMonsterTailC3A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// b28d-52 / SweepBattleMonsterC3A / L5cBattleMonAch 判词表（fresh check_tu_routed，
// 四数 101/156/2/2/55 → 101/158/0/2/55；gap = ORIG−OUR）：
//   get_hellparty_groupOrder_map 0x0836379a —— gap 0（identical/ae）
//   getAddChampionMobCnt        0x083010ee —— gap 0（identical/ae）
//   ConsistTournament           0x083035c8 —— **NEAR → AE**（197/197，gap 0）：三个
//     cMyTrace 全部改全表达式临时链（P221 扩展，具名形 4.4 恒"先声明浅"、槽向与
//     ORIG 相反；ORIG trace 槽 -0x50/-0x40 浅于容器槽 -0x78..-0x5c）。
//   add_hell_party_monster      0x083071ba —— **NEAR → AE**（298/299，gap −1，差值
//     仅尾部对齐 nop）：seq++/uid++ 后置自增形复现 ORIG `mov (%eax),%eax;
//     mov 0xc(%ebp),%edx; mov %eax,(%edx); lea 0x1(%eax),%edx`（P198/P0 值经
//     %eax/%edx 直传；裸 register 中转 %ebx、表达式直用双载 303 条均劣，见下）。
//   checkClearedNamedMonsterMap 0x08300c76 —— **DIFF 80/73 → DIFF 80/80（gap 0）**：
//     ①P0 补 ORIG 有槽的两个具名 int 局部 posX/posY（-0x10/-0xc；原先内联实参被
//       4.4 右起求值 ⇒ 序反 + 少 4 条）；
//     ②P214① 正极性包裹：`m_visited == 0` 负极性让 4.4 出 `xor $1;test;je 主流程`
//       （多 1 条、布局反）；改 `!= 0` 包裹后 ORIG 的 `movzbl 0x1(%eax),%eax;
//       test %al,%al; je <fail 岛>`、push esi、帧 0x380、esi 路径标志
//       （mov $1,%ebx; mov $0,%esi / mov $1,%esi）逐条复现。
//     唯一残差 = 尾测 `cmp $0x1,%esi; jne` ↔ ORIG `test %esi,%esi; je`（值域
//     {0,1} 语义等价；4.4 把 bool 路径标志按 `!= true` 物化，试 `!=0`/(int) cast/
//     return 1/块内 return false 四形均不翻）⇒ N 封顶。
// ============================================================================
#include <map>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "DataManagerInitDepends.h"   // stHellParty_t/stHellPartyMonsterGroupList（须在 CDataManager.h 之后）
#include "HellPartyGroupOrder.h"

// [prett-A 2026-09-11] ORIG prett 串第 2 参为 `MOB_REF_LIST&`（命名空间级 typedef ⇒ 宏打裸名；
// 同型先例 BattleMonsterDepsC2/add_monsters_current_map.inc:5）。纯类型别名，零 codegen。
typedef std::list<_mapMonster> MOB_REF_LIST;

#include "BattleMonsterTailC3A/ConsistTournament.inc"


#include "BattleMonsterTailC3A/checkClearedNamedMonsterMap.inc"


#include "BattleMonsterTailC3A/get_hellparty_groupOrder_map.inc"


#include "BattleMonsterTailC3A/add_hell_party_monster.inc"


#include "BattleMonsterTailC3A/getAddChampionMobCnt.inc"

