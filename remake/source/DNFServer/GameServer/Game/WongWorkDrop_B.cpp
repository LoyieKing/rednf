// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkDrop_B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "WongWorkDrop_B/setDropRatio.inc"


#include "WongWorkDrop_B/setDropRatio_1.inc"


#include "WongWorkDrop_B/generateSpecificItem.inc"


#include "WongWorkDrop_B/hellPartySpecificItem.inc"


#include "WongWorkDrop_B/_generateRandomItem.inc"


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

#include "WongWorkDrop_B/generateEquipGrade.inc"


#include "WongWorkDrop_B/generateUpgrade.inc"


#include "WongWorkDrop_B/chooseItem.inc"



#include "WongWorkDrop_B/_getGenItemProbIdx.inc"


#include "WongWorkDrop_B/_getGenItemProbIdx_1.inc"


}  // namespace WongWork
