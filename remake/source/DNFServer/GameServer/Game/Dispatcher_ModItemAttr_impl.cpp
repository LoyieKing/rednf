// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_ModItemAttr_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Dispatchers.h"

// ============================================================================
// Dispatcher_ModItemAttr::dispatch_sig  —— 还原 ORIG 0x08200b08 T（约 829 insn）
//
// 语义（依据 docs/class_func_reports/Dispatcher_ModItemAttr/dispatch_sig.md）：
//   客户端请求「修改装备属性（封印再铸 / 一次性再铸）」。包格式：
//     get_short  → equipSlot（目标装备槽，INVEN_TYPE_EQUIP=1）
//     get_int    → itemIdx （目标装备 item idx）
//     get_short  → matSlot （消耗材料槽：封印卷 / 再铸材料）
//   校验链：state==3 且当前角色存在 → ServiceRestrict(1,0xf) → 安全保护(0x26)
//           → CheckInTrade → 目标装备非空/ItemType 10..0x15/匹配 idx →
//           → 材料非空 → CheckItemLock(1,equipSlot) → 按材料 item idx 选再铸模式
//           → 扣材料（delete_item）→ 修改装备（update_item）→ _SendResult 回包
//           → 会员返还材料（GetReturnItemRate / insertItemIntoInventory）。
//
// ORIG 的全部校验失败均为「抛出 int / ENUM_ERROR 异常 → 函数尾部 catch」：
//   ENUM_ERROR（正值）→ catch1：SendCmdErrorPacket(0x54, 码&0xff) 后 return 0；
//   int(0)            → catch2：e>=0 → return 0（不发包）；
//   int(-1/-2/-3)     → catch2：e<0 → LineFunc(0x9706,__PRETTY_FUNCTION__,0,0)（仅日志）。
//   本实现用等价的 SendCmdErrorPacket / return / LineFunc 直接表达。
// ============================================================================

#include "InterfacePacketBuf.h"      // InterfacePacketBuf（PacketGuard 基类，put_*）
#include "PacketGuard.h"             // PacketGuard 权威声明（CUser::Send(PacketGuard&) 参数类型）
#include "CDataManager.h"            // G_CDataManager、find_item、reseal_table_t、CItem
#include "CInventory.h"              // INVEN_TYPE / eItemDelReason / eItemAddReason、
                                     // CSecu_ProtectionField、GlobalData::s_pSecuProtectionField
#include "GlobalData.h"              // s_pSecuProtectionField
#include "ServiceRestrictManager.h"  // ServiceRestrictManager
#include "Arad_DataManager.h"        // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "CItemList.h"               // get_rand_int(int)
#include <algorithm>                 // std::min<int>
// LineFunc（包分发断言日志；权威声明 CompoundEmblemDeps.h:35）
extern int LineFunc(int line, const char* func, int errNo, unsigned int mid);


// [AnonLane2 2026-09-11 匿名轴] 旧匿名自由函数
// `_ZN12_GLOBAL__N_111sSendResultEP5CUserttt`（ORIG 全库零命中，§4.1 发明符号）
// 删除：全仓零调用点（调用侧用真实成员
// `Dispatcher_ModItemAttr::_SendResult`，声明 Dispatchers.h:491、定义
// Dispatchers.cpp，mangled `_ZN22Dispatcher_ModItemAttr11_SendResultEP5CUserttt`
// 与 ORIG T 0x08201938 同名；本 .o 对其持 U 引用）⇒ 纯死代码（P154 第二类）。

#include "Dispatcher_ModItemAttr_impl/dispatch_sig.inc"


#include "Dispatcher_ModItemAttr_impl/process.inc"


#include "Dispatcher_ModItemAttr_impl/read.inc"
