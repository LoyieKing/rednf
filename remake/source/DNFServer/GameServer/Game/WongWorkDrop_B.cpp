// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkDrop_B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [b28d-38 判词] 本 TU 8 件最终状态（p167 全部 CLEAR）：
//  _getGenItemProbIdx(Hell/Special) 0x08535a88/0x0853736e DIFF（P183 去 probs 引用缓存；
//   残差 N4/P30 v[0]<=value 的 cmp 极性/disp 折叠）
//  setDropRatio(Hell/Special) 0x08535948/0x085359e8 DIFF（helper call→物理内联 P165，
//   语义补全 14→53 条；残差 N4 v[2+j] 的 +2 并入索引不可折叠）
//  generateSpecificItem 0x08535726 DIFF（m_itemList 具名成员直访消 getItemList call；gap 6）
//  hellPartySpecificItem 0x08535b08 DIFF（byte 层 N 封顶）
//  chooseItem 0x08534fce DIFF（size()==0 / it-- 后置 / 先声明后赋值 P50；N88 残差）
//  _generateRandomItem 0x08536882 DIFF（AMC 链式重发 + 花括号零初始化 P253；gap 11）
// WongWorkDrop_B.cpp — WongWork 掉落生成器批 B（LinkClosureB.WongWorkDrop）
//
// 本 TU 为唯一 owner，逐报告/objdump 忠实转写以下 8 个 ORIG 符号
// （WongWorkDrop_A.cpp 中对应 asm 桥在链接期由本 TU 的真定义消解，A 不改动）：
//   CMonsterDrop_Hell::setDropRatio(float)                    T 0x08535948
//     _ZN8WongWork17CMonsterDrop_Hell12setDropRatioEf
//   CSpecialMonsterDrop::setDropRatio(float)                  T 0x085359e8
//     _ZN8WongWork19CSpecialMonsterDrop12setDropRatioEf
//   CMonsterDrop_Hell::generateSpecificItem(ref,result)       T 0x08535726
//   CMonsterDrop_Hell::hellPartySpecificItem(ref,result)      T 0x08535b08
//   CSpecialMonsterDrop::_generateRandomItem(...)             T 0x08536882（0xAE9 字节）
//   GenerateRandomUpgrade::generateEquipGrade(int)            T 0x08533932
//   GenerateRandomUpgrade::generateUpgrade(ENUM_RARITY,int)   T 0x08533676
//   CSecialIMonsterDropItemGenRateTable::
//       chooseItem(type,rarity,level,diff,ddt)                T 0x08534fce
//
// 报告：docs/class_func_reports/WongWork__CMonsterDrop_Hell/{setDropRatio,
// generateSpecificItem,hellPartySpecificItem}.md、WongWork__CSpecialMonsterDrop/
// _generateRandomItem.md、WongWork__GenerateRandomUpgrade/{generateEquipGrade,
// generateUpgrade}.md；缺失段全部以 objdump df_game_r 兜底。
//
// ::roundFloat（W 0x0853abe5）已由 CClearReward.cpp 定义，本 TU 只 extern 引用。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include <utility>
#include <vector>

#include "CItemGenerator.h"
#include "CItemGeneratorMgr.h"
#include "CDataManager.h"
#include "CMonster.h"                    // IsHellMonster/generateIndependentItem/getMonsterScript
#include "ItemFarmingData.h"             // ItemFarmingData（ORIG W C1/C1ERKS/D1；农田权重条目）
#include "Inven_Item.h"
#include "STMonsterScript.h"            // m_independentDrops(+0x3ac)/m_catchItems(+0x70)
#include "MonsterIndependentDrop.h"     // CAICharacter 权威镜像（generateIndependentItem）

// drops 域公共依赖 ::roundFloat（ORIG W 0x0853abe5；定义 CClearReward.cpp，
// 全局命名空间符号 _Z10roundFloatf —— 放 WongWork ns 外保证调用 mangled 正确）。
extern int roundFloat(float v);

namespace WongWork
{

// ---------------------------------------------------------------------------
// P6 清桥后本 TU 无 __asm__ 桥：下述符号全部为真实成员/函数调用。
//   CMonsterDrop_Hell/CSpecialMonsterDrop::_getGenItemProbIdx → 自身成员内直呼；
//   CLuckPoint::{UseLuckPoint4int,sendItemDropRate} → 定义 CClearReward.cpp；
//   CMonster/CAICharacter 头像掉落、捕获包装与 IsHellMonster → CMonster.h /
//   MonsterIndependentDrop.h 权威声明（IsHellMonster 返回 bool，P292 判据）。
// ---------------------------------------------------------------------------

namespace {

// stGenerateRefData_t +0x34 起调用方上下文区的组件视图：ORIG 0x08535726/0x08535b08
// 均有具名指针局部（`mov 0xc(%ebp),%eax; add $0x34,%eax; mov %eax,-0x2c(%ebp)`）并
// 经它读 +0x35/+0x36/+0x38（P161 组件视图；裸偏移禁止）。布局照 +0x34..+0x3b。
struct stGenerateRefCtx_t
{
    char m_field34;                 // +0x34
    char m_field35;                 // +0x35
    char m_rateSlot;                // +0x36
    char m_pad37;                   // +0x37
    int  m_rankBonusIndex;          // +0x38
};

}  // namespace

// ---------------------------------------------------------------------------
// setDropRatio — ORIG T 0x08535948 / 0x085359e8（两者同型，报告逐条对应）：
//   vector 赋值 m_probsSub(+0x24) = m_probs(+0x18)，随后逐元素 j=0..4：
//   fild(v[j]) * ratio → roundFloat → 存回同一槽位（元素步长 8 ⇒ v[2+j]）。
// ---------------------------------------------------------------------------
#include "WongWorkDrop_B/setDropRatio.inc"
}

#include "WongWorkDrop_B/setDropRatio_1.inc"
}

// ---------------------------------------------------------------------------
// generateSpecificItem — ORIG T 0x08535726（虚槽 +0x0c）
// b21 同族：u8_at/i8_at 全部改 CItemGenerator.h 成员直读（m_field35/m_rateSlot/
//   m_field3d/m_field3e_byte/m_field42_byte，+0x42 按 ORIG movsbl 取有符号），
//   消除 helper 的 EXTRA add+movzbl；残余寄存器分配/栈槽 N-86b + AE。
// ---------------------------------------------------------------------------
#include "WongWorkDrop_B/generateSpecificItem.inc"
}

// ---------------------------------------------------------------------------
// hellPartySpecificItem — ORIG T 0x08535b08
// b21 同族：如 generateSpecificItem 注，成员直读替换 u8_at/i8_at（+0x3d 有符号 movsbl）。
// ---------------------------------------------------------------------------
#include "WongWorkDrop_B/hellPartySpecificItem.inc"
}

// ---------------------------------------------------------------------------
// _generateRandomItem — ORIG T 0x08536882（CSpecialMonsterDrop，报告分段转写；
// 生成率三表偏移 0xa8/0xf8/0x15c 与 loadScript 写入证据一致）。
// ---------------------------------------------------------------------------
#include "WongWorkDrop_B/_generateRandomItem.inc"
}

// ---------------------------------------------------------------------------
// GenerateRandomUpgrade — 升级随机源（m_rand = new CMTRand(0x9c8)）。
// 比率/农田表位于 CDataManager 未建模字段（ORIG lea 0x528(%eax)/lea 0x540(%eax)
// 直接证据；本批禁改 CDataManager.h，故以裸基址偏移访问【裸偏移保留】）：
//   +0x528 map<int, ItemFarmingData>{ vector<pair<int,int>> @0 ; int active @0xc }
//     （布局依 copy ctor 0853ac12：vector op= 后拷 +0xc int，sizeof 0x10）
//   +0x540 vector<pair<int,float>>（level → 比率系数）
// ---------------------------------------------------------------------------
namespace {

typedef std::map<int, ItemFarmingData> FarmingMap;   // 元素步长与 ORIG map 一致

}  // namespace

// ---- generateEquipGrade — ORIG T 0x08533932 -------------------------------
// grades {0x5aa7,0xfa6,0x6a23,0x5df0,0x7270}；weights 全 1000，total=5000；
// w[0] 截断缩放后 total 修正为 5000-w0+w0'，randInt(&total) 落累计权重槽。
#include "WongWorkDrop_B/generateEquipGrade.inc"
}

// ---- generateUpgrade — ORIG T 0x08533676 ----------------------------------
#include "WongWorkDrop_B/generateUpgrade.inc"
}

// ---- CSecialIMonsterDropItemGenRateTable::chooseItem — ORIG T 0x08534fce ---
// type==2（random_option 槽）先试 m_optHandle.choose_item(rarity,level)，命中
// 即返；否则按 diff/ddt 选块（ORIG 槽址 = type*0x7110+rarity*0x12d8+level*24，
// type 调用方传 1..4 ⇒ m_slots[type-1]，m_slots 自块 +0x7110 起）：
// empty→-1；roll=m_rand(+0x211fb0)->randInt(maxKey)；
// lower_bound==end 回退 --it 返 second。
#include "WongWorkDrop_B/chooseItem.inc"
}


 // ---------------------------------------------------------------------------
// _getGenItemProbIdx —— ORIG T 0x08535a88（Hell）/ T 0x0853736e（Special），
// objdump 逐指令转写：遍历 m_probs(+0x24)，命中条件 v[0]<=value && v[1]>=value
// （cmp (elem),value jg 跳过；cmp elem1,value jge 命中）；i>=size 返回 -1。
// ---------------------------------------------------------------------------
#include "WongWorkDrop_B/_getGenItemProbIdx.inc"
}

#include "WongWorkDrop_B/_getGenItemProbIdx_1.inc"
}

}  // namespace WongWork
