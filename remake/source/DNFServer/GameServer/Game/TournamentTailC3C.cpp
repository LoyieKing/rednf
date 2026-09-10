// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TournamentTailC3C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// TournamentTailC3C.cpp — BattleMonsterTailC3A 引入的传递依赖（本 TU 独占实现）
//
// 覆盖符号（ORIG df_game_r nm 实证）：
//   _ZN13CBattle_Field18MakeBasicRoundInfoEiRSt6vectorI17TournamentMonsterSaIS1_EERS0_INS_14TournamentInfoESaIS5_EE
//       CBattle_Field::MakeBasicRoundInfo      @ 0x08303896 T
//       报告：docs/class_func_reports/CBattle_Field/MakeBasicRoundInfo.md
//   _ZN13CBattle_Field16ChoiceFirstGroupERSt6vectorINS_14TournamentInfoESaIS1_EES4_
//       CBattle_Field::ChoiceFirstGroup        @ 0x08303a1c T
//       报告：docs/class_func_reports/CBattle_Field/ChoiceFirstGroup.md
//   _ZN13CBattle_Field22ConsistTournamentRoundERSt6vectorINS_14TournamentInfoESaIS1_EES4_RSt3mapIsS3_St4lessIsESaISt4pairIKsS3_EEERSt4listI11_mapMonsterSaISF_EE
//       CBattle_Field::ConsistTournamentRound  @ 0x08303ace T
//       报告：docs/class_func_reports/CBattle_Field/ConsistTournamentRound.md
//   _Z32Get_instance_groupOrder_map_poolv
//       Get_instance_groupOrder_map_pool       @ 0x089e813f T（局部静态
//       hellPartygroupOrderMapPool_ @ 0x094fe440 b / guard @ 0x094fe438 b）
//       报告：docs/class_func_reports/_lt_global_gt_/Get_instance_groupOrder_map_pool.md
//   _ZN12CDataManager20find_hellparty_groupEi
//       CDataManager::find_hellparty_group     @ 0x08363716 T
//       报告：docs/class_func_reports/CDataManager/find_hellparty_group.md
//
// 依赖（均由已登记 TU 提供）：InsertTournamentInfo/MakeRoundInfo
// （CBattle_Field_unresolved.cpp）、TournamentInfo ctor/Clear/operator<
// （CBattle_Field.cpp）、get_rand_int（GameRand）、Get_instance_stHellParty_t
// （CDataManager.cpp 静态单例）。原 CBattle_Field_unresolved.cpp 中三处带
// clear()/边界防护偏差的临时定义已删除，由本 TU 按报告忠实转写接管。
// ============================================================================

#include <algorithm>
#include <list>
#include <map>
#include <vector>

#include "CDataManager.h"
#include "DataManagerInitDepends.h"  // stHellParty_t/stHellPartyMonsterGroupList（须在 CDataManager.h 之后）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "GameRand.h"                // get_rand_int
#include "HellPartyGroupOrder.h"

// CDataManager.cpp 单例（ORIG 0x089097c4），返回静态缓冲区指针。
extern int* Get_instance_stHellParty_t();

// ---------------------------------------------------------------------------
// CBattle_Field::MakeBasicRoundInfo @ 0x08303896
// 报告要点：random_shuffle(monsters)；begin()==end() 直接返回 false；
// while(round<=14 && it!=end)：每轮构造 TournamentInfo，内层 for(j<memberCount)
// 复制 *it 到 m_monster[j] 并 m_group += it->m_rate(+4)、++it，push_back 后 ++round；
// 退出时返回 round==15。ORIG 内层循环不做 end 检查、无 memberCount 边界检查，
// roundInfo 不预先 clear —— 全部忠实保留。
#include "TournamentTailC3C/MakeBasicRoundInfo.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::ChoiceFirstGroup @ 0x08303a1c
// 报告要点：先取 src 首元素 m_group 作基准（无条件解引用 begin()）；随后
// while(it!=end && it->m_group==group)：dst.push_back(*it)、it=src.erase(it)。
// ORIG 不清空 dst、无空表防护 —— 忠实保留。
#include "TournamentTailC3C/ChoiceFirstGroup.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::ConsistTournamentRound @ 0x08303ace
// 报告要点：randIdx=get_rand_int(0x10)；先插入键 randIdx 的空 TournamentInfo；
// 偶数 peer=randIdx+1 否则 randIdx-1；shuffle(first) 后无条件改首元素
// m_roundOrder=peer 并插入副本再 erase(begin())；indexes 收集 [0,0x10) 中除
// randIdx/peer 外的下标并 shuffle；InsertTournamentInfo 先 first 后 second；
// 最后 MakeRoundInfo(tournament, roundMap, mobList)。ORIG 对空 first 无防护 —— 忠实保留。
#include "TournamentTailC3C/ConsistTournamentRound.inc"
}

// ---------------------------------------------------------------------------
// Get_instance_groupOrder_map_pool @ 0x089e813f
// 报告要点：__cxa_guard 保护的函数局部静态 map<int,map<unsigned short,
// STHellPartyGroupOrder>>（hellPartygroupOrderMapPool_）+ __cxa_atexit 析构注册，
// 返回其地址 —— 与 C++ 函数局部静态语义一一对应。
HellPartyGroupOrderMapPool* Get_instance_groupOrder_map_pool()
{
    static HellPartyGroupOrderMapPool hellPartygroupOrderMapPool_;
    return &hellPartygroupOrderMapPool_;
}

// ---------------------------------------------------------------------------
// CDataManager::find_hellparty_group @ 0x08363716
// 报告要点：key=(unsigned short)idx；在 Get_instance_stHellParty_t() 单例
// +0x54 的 map<unsigned short,stHellPartyMonsterGroupList> 上 find；
// 未命中返回 NULL，命中返回 &it->second（pair 内偏移 +4，与 ORIG
// operator->+0x4 一致）。调用方（BattleMonsterTailC3A.cpp）将该指针视图化为
// stHellPartyMonsterGroupList* 使用。
#include "TournamentTailC3C/find_hellparty_group.inc"
}
