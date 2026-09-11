#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_BuyCeraShopItem_impl/<Func>.inc（不参与编译、内部不含 #include）。
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
#include "GlobalData.h"
#include "CDataManager.h"
#include "CUserCharacInfo.h"
#include "CItemList.h"
#include "CItem.h"
#include "GameWorld.h"
#include "GameRand.h"
#include "GameTypes.h"
#include "LogManager.h"
#include "Arad_DataManager.h"
#include "ServiceRestrictManager.h"
#include "SigTypes.h"
#include "CParty.h"
#include "CEquipItem.h"
#include "PacketGuard.h"        // PacketGuard（ORIG -0x80 槽 ctor/dtor + eh cleanup）
#include "CPrivateStoreMgr.h"   // private_store::CPrivateStoreMgr/CPrivateStore 权威声明
                               //（IsBusyPrivateStore 0x085cad5e / FindStoreSeller 0x085ca358 /
                               // isDollCreated 0x082345e4；TU 局部最小声明已删除）
class CDungeon;  // 仅 CDungeon* 指针使用，前向声明即可；避免引入 CDungeon.h 触发 QuickParty 重复定义

// ============================================================================
// Dispatcher_BuyCeraShopItem::dispatch_sig 还原
//   ORIG _ZN26Dispatcher_BuyCeraShopItem12dispatch_sigEP5CUserR9PacketBuf @ 0x081fd396 T
// 报告：docs/class_func_reports/Dispatcher_BuyCeraShopItem/dispatch_sig.md
// 辅助：docs/class_func_reports/Dispatcher_BuyCeraShopItem/useCountDownCoinInFreeCoinDungeon.md
//
// 【依赖说明】下列 ORIG 符号当前重建尚无 producer（未派发/未还原/无权威头）。
// 本 TU 内只做「局部最小声明（推断）」并调用，绝不伪造实现体，也不重复定义。
// 依赖 producer 落定后应改为 include 权威头 + 真实调用，并删除本地声明。
//   blocked_dependencies（已向 Main 上报）：
// B1 WongWork::CCeraShop::ProcessError     _ZN8WongWork9CCeraShop12ProcessErrorEP5CUsertijiic @0x8323978  (无 producer)
//   B2 ItemVendingMachine::BuyItem           _ZN18ItemVendingMachine7BuyItemEP5CUserRK21MSG_BUY_CERASHOP_ITEM @0x854c4d0  (无 producer)
//   B3 private_store::CPrivateStore::isDollCreated _ZN13private_store13CPrivateStore13isDollCreatedEv @0x82345e4  (声明改由 CPrivateStoreMgr.h 权威头提供)
//   B4 CSecu_ProtectionField::GetCashShopErr  _ZN21CSecu_ProtectionField14GetCashShopErrEi @0x82881a2  (头未声明)
//   B5 LineFunc                             _Z8LineFunciPKcij @0x85908d7  (无权威头)
//   B6 MSG_CHECK_MAIL_RECEIVER::set<...>     模板无 producer
//   B7 CEquipItem::getAvatarOptionSize       _ZNK10CEquipItem19getAvatarOptionSizeEv @0x81803bc  (头未声明)
//
// 注：useCountDownCoinInFreeCoinDungeon 为同类成员但未在只读头 Dispatchers.h 声明，
//   本 TU 无法产出该成员符号；其逻辑按报告内联（useCountDownCoinInFreeCoinDungeon_inline）。
// ============================================================================

// CCeraShop 权威声明见 CCeraShop.h；CMailBoxHelper 权威声明见 CGameManager.h
//（均经下方 ItemVendingMachine.h / CGameManager.h include；此处不再重复定义，避免 ODR）。
// ::ItemVendingMachine（权威声明见 ItemVendingMachine.h；BuyItem 返回 int，matches ORIG）。
#include "ItemVendingMachine.h"

// MSG_CHECK_MAIL_RECEIVER 为全局作用域权威定义（CGameManager.h，ORIG _ZN23MSG_CHECK_MAIL_RECEIVER..无 WongWork），
// 含 set<ENUM_MAILBOX_CHECKTYPE,MSG_T> 模板。WongWork::CMailBoxHelper::ReqDBCheckReceiver 在 WongWork 命名空间。
#include "CGameManager.h"

// CSecu_ProtectionField（GlobalData.h 仅前置；Check/GetCashShopErr 取成员形态符号）。
#ifndef DNF_ENUM_SECURITY_PROTCTION_DEFINED
#define DNF_ENUM_SECURITY_PROTCTION_DEFINED
enum SECURITY_PROTCTION
{
    SECURITY_PROTCTION_0 = 0,
    SECURITY_PROTCTION_0x16 = 0x16
};
#endif
// CSecu_ProtectionField 权威完整定义见 CInventory.h:400 区（本地最小镜像已删，避免 ODR）。

// LineFunc / NumberToString（ORIG 自由函数，权威头缺失但符号真实存在）。
extern int LineFunc(int, const char*, int, unsigned int);
extern char* NumberToString(unsigned int value, int radix);

// CEquipItem::getAvatarOptionSize 权威声明见 CEquipItem.h（ORIG W 0x81803bc）。
// 本 TU include "CEquipItem.h"，不做局部重定义。

// ============================================================================
// dispatch_sig 条目访问视图：SigTypes_A.h MSG_BUY_CERASHOP_ITEM（0x3f84）之
//   MSG_CERA_SHOP_BUY_SCALARS（+0x0d buytype / +0x0f buyFlag / +0x10..0x2d 收件人名 /
//   +0x36 itemCount）与 MSG_CERA_SHOP_BUY_ENTRIES（+0x30 起、步长 0x1cf 条目，
//   字段 +0x07/+0x08/+0x09/+0x0d/+0x0e stride5/+0xbd/+0xbe stride8）。
// 原「本地 stBuyCeraEntry[0x24]（步长 0x250）+ memset + 尾部重组 buyMsg」方案与
// ORIG 帧形不符（ORIG 全程原地读入唯一消息对象、无 memset、尾块直接传该对象），
// 已删除；证据 docs/class_func_reports/Dispatcher_BuyCeraShopItem/dispatch_sig.md
// ORIG 汇编 0x081fd396..0x081fe4b7。
//
// 【§6.2 巨件核销】P167 96/96 [0缺0多]；gap 116→-9；残差归因 N82
// （o=1002 n=1011；调用点差集 96/96 归零，仅 cMyTrace/PacketGuard C1↔C2、D1↔D2
// ORIG 同址别名形态差）
// ；msg 基址 -0x4008、
// 槽 msg+0x0d/0x0f/0x36/0x39 绝对位与 ORIG 全同，帧 0x7fb0↔0x7fbc（push %edi 差）。
// 残余 9 指令与行级 hunk 为 N82 槽号/寄存器分配 + eh 清理复用形态 ⇒ 判语义完成、
// byte 层 N 封顶。
// ============================================================================

#include "Dispatcher_BuyCeraShopItem_impl/dispatch_sig.inc"


#include "Dispatcher_BuyCeraShopItem_impl/process.inc"
#include "Dispatcher_BuyCeraShopItem_impl/read.inc"
