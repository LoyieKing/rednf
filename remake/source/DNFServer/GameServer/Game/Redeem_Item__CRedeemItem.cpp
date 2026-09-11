#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Redeem_Item__CRedeemItem/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Redeem_Item::CRedeemItem —— 恢复实现（独立 TU）。
// 语义依据 docs/class_func_reports/Redeem_Item__CRedeemItem/（ORIG 反汇编 + Ghidra C）。
// 仅实现 CUser::Add_RedeemInfo 调用的两个方法 + 私有辅助 _isFPItem；
// 其余 ORIG 方法（ctor/dtor/loadData/_saveData/GetItemPrice/RedeemItem 等）未建模。

#include "Redeem_Item__CRedeemItem.h"

#include "CDataManager.h"
#include "CItem.h"
#include "GameWorld.h"        // G_GameWorld / IsEnchantRevisionChannel
#include "PacketGuard.h"

namespace Redeem_Item
{

// ORIG 命名空间内 static 常量（_ZN11Redeem_ItemL...E，r 段；值实测
// 0x08cd21c8..）：MAX_COUNT=21 / THROW_FEE=1000 / BUY_FEE=1000。
static const int MAX_COUNT = 21;
static const int THROW_FEE = 1000;
static const int BUY_FEE = 1000;

// ORIG 0x085f7c04：itemId 在 (0x1963, 0x1b57] 区间 → true（"FP 物品"判定）。

#include "Redeem_Item__CRedeemItem/_isFPItem.inc"


#include "Redeem_Item__CRedeemItem/isAddableFilter.inc"


#include "Redeem_Item__CRedeemItem/AddRedeemList.inc"


#include "Redeem_Item__CRedeemItem/getData.inc"



#include "Redeem_Item__CRedeemItem/Send_RedeemList.inc"


}  // namespace Redeem_Item
