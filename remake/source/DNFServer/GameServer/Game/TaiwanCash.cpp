// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TaiwanCash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "TaiwanCash.h"
#include <cstdio>
#include <cstring>

#include "CEnvironment.h"  // G_CEnvironment / STDBConnInfo / DB_HANDLE_E（InitBillingCash）
#include "LogManager.h"    // Initialize 失败路径 logFormat（ORIG IPGMgr.cpp 0x98 行文案）
#include "DNFFunctionLib.h"  // NumberToString(ull,int)（billing trace 直调）

#include "TaiwanCash/InitBillingCash.inc"


namespace Taiwan
{

#include "TaiwanCash/PurchaseQuery.inc"


#include "TaiwanCash/setQuery.inc"

#include "TaiwanCash/execQuery.inc"


#include "TaiwanCash/getResult.inc"


// [SplitC 2026-09-11 batch3 · TU 迁移] TaiwanCash 已迁出至 TaiwanCashOrig.cpp（ORIG GI 键对齐）。



#include "TaiwanCash/init.inc"


#include "TaiwanCash/increaseRequest.inc"


#include "TaiwanCash/checkResultCode.inc"


#include "TaiwanCash/purchase.inc"


}  // namespace Taiwan

// explicit instantiation → ORIG W 符号 _ZN6Taiwan10TaiwanCash14processBillingINS_13PurchaseQueryEEEbRT_
template bool Taiwan::TaiwanCash::processBilling<Taiwan::PurchaseQuery>(
    Taiwan::PurchaseQuery&);

// ============================================================================
// WongWork::IPG::CIPGMgr —— 权威布局见 TaiwanCash.h；逐函数 ORIG 转写。
// 依据：docs/class_func_reports/WongWork__IPG__CIPGMgr/{Initialize,Finalize}.md
// 及 objdump 实测（ctor 0x08101108、dtor 0x08101222、GetIPGHelper 0x081053c6）。
// ============================================================================

// ---- CIPGMgr::CIPGMgr —— ORIG W 0x08101108：
//   *(this+0)=0；两个 queue<SIPGData*>（临时 deque move 构造，等价默认构造）
//   落 +0xc/+0x34；DynamicPool<SIPGData>(1000)@+0x5c；helper@+0xb8；
//   Mutex@+0xbc/+0xd4 自动构造。billingCash(+0xec) 不在此初始化（ORIG 同）。----
namespace WongWork { namespace IPG {

#include "TaiwanCash/CIPGMgr.inc"


#include "TaiwanCash/dtor_CIPGMgr.inc"


#include "TaiwanCash/Initialize.inc"


#include "TaiwanCash/getBillingCash.inc"

 }}

#include "TaiwanCash/CNode.inc"


#include "TaiwanCash/CheckBillingErrorCode.inc"


// ============================================================================
// Wave5 仅ORIG批 —— TaiwanCash 新增 9 方法（追加块，不改上方既有实现）。
// 依据 docs/class_func_reports/Taiwan__TaiwanCash/{getBalance,getBalance_0817381a,
// checkEntrance,gift,cancel,charge,confirm,purchase,ceraPoint}.md 汇编逐条转写。
// Query 完整类型来自 TaiwanQueries.h（TwQuery 批次）；各 processBilling<X>
// 显式实例化 → ORIG 独立 W 符号 _ZN6Taiwan10TaiwanCash14processBillingINS_IXXXEEEbRT_。
// NumberToString(y,i) @0x08109098 以 snprintf 等价替代【推断：等价日志面，
// 沿用本 TU purchase 既有模式】。所有 SQL/存储过程文本在各 Query 归属批内。
// ============================================================================

#include "TaiwanQueries.h"

namespace Taiwan
{

#include "TaiwanCash/getBalance.inc"


#include "TaiwanCash/getBalance_1.inc"


#include "TaiwanCash/checkEntrance.inc"


#include "TaiwanCash/gift.inc"


#include "TaiwanCash/cancel.inc"


#include "TaiwanCash/charge.inc"


#include "TaiwanCash/confirm.inc"


#include "TaiwanCash/purchase_1.inc"


#include "TaiwanCash/ceraPoint.inc"


}  // namespace Taiwan

// ---- processBilling<X> 显式实例化 → ORIG W 符号 ----
template bool Taiwan::TaiwanCash::processBilling<Taiwan::BalanceQuery>(
    Taiwan::BalanceQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::BalancePointQuery>(
    Taiwan::BalancePointQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::EntranceQuery>(
    Taiwan::EntranceQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::GiftQuery>(
    Taiwan::GiftQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::CancelQuery>(
    Taiwan::CancelQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::ChargeQuery>(
    Taiwan::ChargeQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::ConfirmQuery>(
    Taiwan::ConfirmQuery&);
template bool Taiwan::TaiwanCash::processBilling<Taiwan::CeraPointQuery>(
    Taiwan::CeraPointQuery&);
