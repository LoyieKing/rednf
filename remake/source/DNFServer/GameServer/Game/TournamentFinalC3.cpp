// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TournamentFinalC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// TournamentFinalC3.cpp — CBattle_Field 锦标赛链最后两个链接符号（本 TU 独占实现）
//
// 覆盖符号（ORIG df_game_r nm 实证，均为 T）：
//   _ZN13CBattle_Field20InsertTournamentInfoERSt6vectorIiSaIiEERS0_INS_14TournamentInfoESaIS4_EERSt3mapIsS4_St4lessIsESaISt4pairIKsS4_EEE
//       CBattle_Field::InsertTournamentInfo        @ 0x08303e02 T
//       报告：docs/class_func_reports/CBattle_Field/InsertTournamentInfo.md
//   _ZN13CBattle_Field13MakeRoundInfoERSt3mapIsNS_14TournamentInfoESt4lessIsESaISt4pairIKsS1_EEERS0_IsSt6vectorIS1_SaIS1_EES3_SaIS4_IS5_SC_EEERSt4listI11_mapMonsterSaISI_EE
//       CBattle_Field::MakeRoundInfo               @ 0x08303f66 T
//       报告：docs/class_func_reports/CBattle_Field/MakeRoundInfo.md
//   _ZN13CBattle_Field17MakeNextRoundInfoEiRSt3mapIsNS_14TournamentInfoESt4lessIsESaISt4pairIKsS1_EEE
//       CBattle_Field::MakeNextRoundInfo           @ 0x0830410c T
//       报告：docs/class_func_reports/CBattle_Field/MakeNextRoundInfo.md
//   _ZN13CBattle_Field25MakeTournamentMonsterListERSt3mapIsSt6vectorINS_14TournamentInfoESaIS2_EESt4lessIsESaISt4pairIKsS4_EEERSt4listI11_mapMonsterSaISE_EE
//       CBattle_Field::MakeTournamentMonsterList   @ 0x08304594 T
//       报告：docs/class_func_reports/CBattle_Field/MakeTournamentMonsterList.md
//   _ZN13CBattle_Field20AddTournamentMonsterERKNS_14TournamentInfoERSt4listI11_mapMonsterSaIS4_EEb
//       CBattle_Field::AddTournamentMonster        @ 0x08304718 T
//       报告：docs/class_func_reports/CBattle_Field/AddTournamentMonster.md
//
// 后三个为 MakeRoundInfo 的链式依赖（此前唯一定义也在未登记孤儿文件
// CBattle_Field_unresolved.cpp 中，登记后链接必然暴露），一并按报告转写。
// 其余依赖（均由已登记 TU 提供）：get_rand_int（GameRand）、TournamentInfo
// ctor 与 _mapMonster 全套（CBattle_Field.cpp / CMap.cpp）；_mapMonster/
// TournamentMonster 权威布局见 CMap.h。TournamentInfo/TournamentMonster 无
// 析构符号，走平凡析构（与 ORIG 一致）。
// ============================================================================

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h

// ---------------------------------------------------------------------------
// CBattle_Field::InsertTournamentInfo @ 0x08303e02
// 报告要点：并行迭代 infos/indexes —— 循环条件先判 infoIt!=infos.end() 再判
// idxIt!=indexes.end()（0x08303f0c..0x08303f5b），任一到尾即退出；循环体把
// *idxIt 截断为 short 写入元素首字段 m_roundOrder(+0x00)（0x08303e50 mov %ax,
// (%ebx)），随后以该字段自身作键 make_pair(m_roundOrder, *infoIt)（ORIG 两实参
// 均指向同一元素：operator->/operator* 结果一致）经 pair<short,TournamentInfo>
// → pair<const short,TournamentInfo> 转换构造后 map::insert（结果对 -0x48，
// 丢弃），最后 infos.erase/infoIt、indexes.erase/idxIt 双向推进 —— 迭代器仅靠
// erase 返回值前进（0x08303eb0..0x08303ef4），无 ++it、无空表防护 —— 忠实保留。
#include "TournamentFinalC3/InsertTournamentInfo.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::MakeRoundInfo @ 0x08303f66
// 报告要点：round 从 1 计到 4（0x083040d7 cmp $0x4 / setle，含 4）。每轮：
// map::begin 后构造空 snapshot vector（0x08303f7a..0x08303f95），内层 while
// （每轮重取 map::end，0x08303fd6）将全部 second（pair+0x04）push_back；
// 迭代器用后置 it++（ORIG 0x083113d8 三参 operator++(int)，含丢弃的临时拷贝）；
// 插入 roundInfoMap（返回值丢弃），~pair×2 后调 MakeNextRoundInfo(round,
// roundMap) 推进对阵（0x0830410c），最后 ~vector；循环外调
// MakeTournamentMonsterList(roundInfoMap, mobList)（0x08304594）。
// roundMap 不预先 clear —— 忠实保留。
#include "TournamentFinalC3/MakeRoundInfo.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::MakeNextRoundInfo @ 0x0830410c
// 报告要点：函数顶部构造局部 nextMap（0x0831116e）；循环 i=0 起、步进 +2，
// 条件每轮重算 0x10/memberCount-1（0x08304528 idivl 有符号除法，memberCount
// 为 0 时 ORIG 直接除零 —— 无防护，忠实保留）。每对 a=i/b=i+1：先取
// roundMap[(short)a].m_group(+0x14)、roundMap[(short)b].m_group；若
// roundMap[(short)a].m_monster[0].m_mobId(+0x04)==0 则 a 晋级，否则判 b；
// 都不空时 rndA=0;rndB=0 初始化后 get_rand_int(groupA)/get_rand_int(groupB)，
// rndA>rndB 时 a 晋级否则 b。晋级写 roundMap[key].m_roundOrder=(short)(key/2)
// 后以三次 operator[] 形态 insert 进 nextMap：
// insert(make_pair(roundMap[key].m_roundOrder, roundMap[key]))（返回值丢弃）。
// 循环外 roundMap = nextMap（copy-assign 0x083115be）→ ~nextMap。
#include "TournamentFinalC3/MakeNextRoundInfo.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::AddTournamentMonster @ 0x08304718
// 报告要点：i<=1 循环（cmpl $1/setle）；每次迭代先构造 _mapMonster（ctor
// 0830e4fc）再判 info.m_monster[i].m_mobId!=0（param+i*8+4），命中按 ORIG
// 写入顺序填字段：flag04(字节)=1 → field08=0 → levelAdd=1 → field14=1 →
// spawnType=0 → roleType 以 if/else 双分支直接存储（isUltimate?3:0）→ champion(字节)=0 → posX=0 →
// orderIdx=0 → hpPct=100 → flag30(字节)=0（注意：ORIG 不写 m_level(+0x24)，
// 保持 ctor 默认），push_back 后析构局部 —— 空槽同样走 ctor/dtor 对。
#include "TournamentFinalC3/AddTournamentMonster.inc"
}

// ---------------------------------------------------------------------------
// CBattle_Field::MakeTournamentMonsterList @ 0x08304594
// 报告要点：round=1..4（cmpl $4/setle）；每轮 vec=roundMap[(short)round]
// （operator[] 083115dc）；peer 初值 -1，内层 i=0..15 找首个
// m_monster[0].m_mobId==0 的下标，偶数 peer=i+1 否则 peer=i-1 后 break；
// 未命中保持 peer=-1 仍无条件 AddTournamentMonster(vec[peer],...)（负下标
// UB —— ORIG 无防护，忠实保留）；round==4 传 true 否则 false。
// 循环后遍历 mobList 重编号 it->m_orderIdx=order++（+0x2c，list 迭代器
// 后置 it++ 推进）。
#include "TournamentFinalC3/MakeTournamentMonsterList.inc"
}
