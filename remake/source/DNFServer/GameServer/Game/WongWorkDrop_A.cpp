// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkDrop_A/<Func>.inc（不参与编译、内部不含 #include）。
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

#include <algorithm>

#include "CItemGenerator.h"
#include "Inven_Item.h"
#include "GameRand.h"
#include "CDataManager.h"
#include "CMonster.h"
#include "STMonsterScript.h"
#include "MonsterIndependentDrop.h"
#include "CDungeon.h"

namespace WongWork
{


}  // namespace WongWork

// ORIG W 0x0853abe5 ::roundFloat —— 定义于 CClearReward.cpp（drops 域公共依赖，
// 全局命名空间符号 _Z10roundFloatf；声明须置于全局域保证调用 mangled 匹配）
extern int roundFloat(float v);

namespace WongWork
{

// ---------------------------------------------------------------------------
// 文件局部 asm 桥（只引用不定义）
// ---------------------------------------------------------------------------
namespace {

// b28d-solo-WongWorkDrop_A：CWorldDrop ORIG 具名指针局部 ctx = &ref+0x34
// 的字节视图（ctx[0]=type/ctx[1]=level）。结构体指针视图使 4.4 出
// `movzbl 0x1(%eax)` 单条 disp（裸 ctx[1] 恒产 add $1 + disp 0，P30/N4）。
struct StRefCtxView
{
    unsigned char m_type;    // +0x34
    unsigned char m_level;   // +0x35
};

// CMonsterDrop::generateSpecificItem 低难度块 ORIG 具名指针 ctx = &ref+0x34
//（-0x20 槽）：ctx[4]=+0x38 int / ctx[8]=+0x3c / ctx[9]=+0x3d。
struct StRefCtx2
{
    unsigned char m_pad[4];      // +0x34..+0x37
    int m_rankBonusIndex;        // +0x38
    unsigned char m_difficulty;  // +0x3c
    unsigned char m_count;       // +0x3d
    unsigned char m_field3e;     // +0x3e
    unsigned char m_field3f;     // +0x3f
};

}  // namespace

#include "WongWorkDrop_A/generateRandomItem.inc"


#include "WongWorkDrop_A/generateRandomItem_1.inc"


#include "WongWorkDrop_A/generateSpecificItem.inc"


#include "WongWorkDrop_A/generateSpecificItem_1.inc"


// ===========================================================================
// CMonsterDrop
// ===========================================================================

#include "WongWorkDrop_A/_getGenItemProbIdx.inc"


#include "WongWorkDrop_A/_getBossGoldDrop.inc"


#include "WongWorkDrop_A/setDropRatio.inc"


#include "WongWorkDrop_A/_generateRandomItem.inc"


#include "WongWorkDrop_A/_generateMonseterSpecificItem.inc"


#include "WongWorkDrop_A/generateRandomItem_2.inc"


#include "WongWorkDrop_A/generateSpecificItem_2.inc"


#include "WongWorkDrop_A/stFirstMobHuntingWeight_t.inc"


}  // namespace WongWork
