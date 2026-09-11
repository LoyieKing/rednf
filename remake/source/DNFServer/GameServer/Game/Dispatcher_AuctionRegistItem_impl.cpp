#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_AuctionRegistItem_impl/<Func>.inc（不参与编译、内部不含 #include）。
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
// Dispatcher_AuctionRegistItem::dispatch_sig —— 还原 ORIG 0x08213e8a T
// （range [0x08213e8a, 0x08214b95]，792 insn；报告
//   docs/class_func_reports/Dispatcher_AuctionRegistItem/dispatch_sig.md
//   汇编段 + Ghidra C 段均已通读）
//
// 语义（CMDPACKET=0xba「拍卖行登记物品」）：包首字节 pay_type。
//   pay==1（cera/点券）→ PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP（ctor ORIG W 0x0822e9b4，
//           sizeof 0x2d，category 0x12/pkid 2）→ CCeraAuctionServerProxy::SendPacket。
//   pay==0（gold/金币）→ PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA（ctor ORIG W 0x0822e54e，
//           sizeof 0x3d，category 0/pkid 2，含 ROI_Category@+0x31）→
//           CAuctionServerProxy::SendPacket。
//   校验链：get_byte 成功 → pay>1 拒 → pay==1 且角色等级<0x1f 拒(0x75) →
//           AuctionPreCheck(0xba,pay)（>=0 直接返回）→ ServiceRestrict(1,5) → Secu(0x32)。
//   GP 分支：读 inven_type/slot_no/item_id/add_info/price/instant_price（无 unit_price、
//           无 ROI）→ IsPriceError/上限、item_id∈[0x28d288,0x28d299]、add_info<=1、
//           price==-1、instant∈[0x32,0x895440] → 发包。
//   GA 分支：多读 unit_price + 3×ROI(short,byte)；find_item；IsRandomOption() 时把空 ROI
//           high 置 0x7530；ROI_Category::_sort()；遍历
//           CDataManager::m_serverParameter.m_auction480 限购表（m_itemId==-1 通配或等于
//           请求 itemId，且 curSec∈(m_startSec,m_endSec)）→ 结余包 header(1,0xba)|0|7|3×int
//           经 CUser::Send 后 return 0；否则 GetInvenTypeFromItemSpace + CheckItemLock(0xd5)
//           → 价格范围 + stackable 乘积校验 → 发包。
//   收尾：遍历文件级静态 gmList[5]（ORIG d _ZL6gmList）命中则
//         cMyTrace("Trace Auction Delay, %s(%d)", "REQ AuctionRegistItem", characNo)。
//
// 说明（2 类型 1 ctor 建模）：ORIG 在两个分支分别构造 *不同* 的包类型，各一次 ctor
// 调用（0x08214042 → GP ctor；0x082143a7 → GA ctor）；ORIG 中不存在独立的
// ROI_Category 局部对象（ROI 直接读进 GA 包 +0x31 字段，ROI_Category ctor 只出现在
// GA ctor 内部），故本 TU 不再自建 RoiCategory / 合并型 PACK_ 结构。
// ============================================================================

#include "AuctionHelpers.h"          // IsPriceError / IsValidMultiplyResult（ORIG T）
#include "AuctionProxy.h"            // CAuctionServerProxy / CCeraAuctionServerProxy::SendPacket
#include "PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA.h"   // ORIG ctor W 0x0822e54e（sizeof 0x3d）
#include "PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP.h"   // ORIG ctor W 0x0822e9b4（sizeof 0x2d）
#include "ROI_Category.h"            // ROI_Category::_sort（ORIG W 0x0822ac30）
#include "CDataManager.h"            // G_CDataManager、find_item、m_serverParameter
#include "ServerParameterScript.h"   // stAuctionLimitItemParam_t（m_auction480，12B）
#include "CInventory.h"              // CSecu_ProtectionField、GetInvenTypeFromItemSpace
#include "CSystemTime.h"             // getCurSec（ORIG 0x080cbc9e）
#include "GlobalData.h"              // s_auction_proxy / s_cera_auction_proxy / s_pSecuProtectionField
#include "ServiceRestrictManager.h"  // ServiceRestrictManager
#include "Arad_DataManager.h"        // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "PacketGuard.h"             // PacketGuard（限购结余包）
#include "InterfacePacketBuf.h"      // put_header / put_byte / put_int / finalize
#include "GameTypes.h"               // cMyTrace
#include "CompoundEmblemDeps.h"      // LineFunc(int, char const*, int, unsigned)

// 拍卖 GM 角色号表（ORIG d _ZL6gmList @0x093701d8，文件级静态，5×int）
static int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

#include "Dispatcher_AuctionRegistItem_impl/dispatch_sig.inc"


#include "Dispatcher_AuctionRegistItem_impl/process.inc"
#include "Dispatcher_AuctionRegistItem_impl/read.inc"
