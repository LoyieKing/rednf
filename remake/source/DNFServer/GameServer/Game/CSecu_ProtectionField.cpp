// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSecu_ProtectionField/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CSecu_ProtectionField 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CInventory.h"
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解环）⇒ 本 TU 解引用 CUser 成员，需自行直补完整定义。
#include "CUser.h"

// ORIG T 0x08288074：枚举名 SECURITY_PROTCTION_TYPE（23 字符，c++filt 实测），
// ORIG mangled _Z23ExceptionProtectionTypei23SECURITY_PROTCTION_TYPE_S_S_S_S_S_S_S_。
enum SECURITY_PROTCTION_TYPE
{
    SECURITY_PROTCTION_TYPE_0 = 0
};

#include "CSecu_ProtectionField/ExceptionProtectionType.inc"


#include "CSecu_ProtectionField/Check.inc"


#include "CSecu_ProtectionField/GetOppositeErr.inc"



#include "CSecu_ProtectionField/GetProtectionType.inc"


#include "CSecu_ProtectionField/GetCashShopErr.inc"

#include "CSecu_ProtectionField/init.inc"


#include "CSecu_ProtectionField/CSecu_ProtectionField.inc"


#include "CSecu_ProtectionField/CheckItem.inc"


#include "CSecu_ProtectionField/CheckItem_1.inc"


#include "CSecu_ProtectionField/CheckRestoreType.inc"


#include "CSecu_ProtectionField/CheckTradingType.inc"


#include "CSecu_ProtectionField/CheckEtcType.inc"


// ===========================================================================
// LinkMisc5 收口（ORIG 反汇编/报告逐条转写）
// ===========================================================================

#include "CSecu_ProtectionField/GetInvenItemPtr.inc"

#include "CSecu_ProtectionField/GetItemPtr.inc"


#include "CSecu_ProtectionField/CheckAccountType.inc"


#include "CSecu_ProtectionField/CheckOpposite.inc"


#include "CSecu_ProtectionField/CheckItemRarity.inc"

