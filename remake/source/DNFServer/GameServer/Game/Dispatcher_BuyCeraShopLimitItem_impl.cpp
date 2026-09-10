// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatcher_BuyCeraShopLimitItem_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Dispatchers.h"

#include <vector>
#include <cstring>

#include "CUser.h"                 // CUser 全套（get_state/GetCera/GetUID/GetServerGroup/
                                   //   IsRestrictedGoods/SetRestrictedGoods/SendCmdErrorPacket/CheckInTrade/getRegTime）
#include "CUserCharacInfo.h"       // getCurCharacInvenR/get_charac_job/level/getCurCharacCreateTime/
                                   //   GetWinPoint/getLastItemLimitEditionBuyTime/updateLastItemLimitEditionBuyTime
#include "CInventory.h"            // get_money + CSecu_ProtectionField::Check + SECURITY_PROTCTION
#include "CPrivateStoreMgr.h"      // private_store::GetInstancePrivateStoreMgr / IsBusyPrivateStore
#include "CDataManager.h"          // G_CDataManager / find_item / getItemList / getItemLimitEditionMgr
#include "CItemLimitEdition.h"     // CItemLimitEdition / CItemLimitEditionMgr / stItemLimitEditionItemInfo_t
#include "CCeraShop.h"             // WongWork::CCeraShop::checkBuyingCondition / IsSpecialItem
#include "GlobalData.h"            // s_systemTime_ / s_pCeraShop / s_monitor_proxy_mgr / s_pSecuProtectionField
#include "CMonitorServerProxy.h"   // CMonitorServerProxy::SendTcpPacket
#include "CServerProxyMgr.h"       // CServerProxyMgr<T>::GetServerProxy
#include "CEquipItem.h"            // stAvatarTypeSelect_t / CEquipItem::m_avatarSelect
#include "CItemList.h"             // CItemList::GetRestrictCode
#include "CSystemTime.h"           // CSystemTime::getCurSec
#include "Packet_Item_Limit_Edition_Buyable_Query.h"  // 权威头（shared/packet）：ORIG _ZN39Packet_Item_Limit_Edition_Buyable_QueryC1Ev @0x0822c064

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T。
// 权威声明见 CompoundEmblemDeps.h；本 TU 用普通 extern 声明（非 asm 桥），定义由 CompoundEmblemDeps.cpp 提供。
extern int LineFunc(int, const char*, int, unsigned int);

// ============================================================================
// Dispatcher_BuyCeraShopLimitItem::dispatch_sig 还原
//   ORIG _ZN31Dispatcher_BuyCeraShopLimitItem12dispatch_sigEP5CUserR9PacketBuf @ 0x0821f9ba T
// 报告：docs/class_func_reports/Dispatcher_BuyCeraShopLimitItem/{dispatch_sig,_checkUserState,_checkBuyCondition}.md
//
// 语义：瑟拉商城「限购版」购买。解析商品 IPG/数量/记录数组 → _checkUserState 校验用户状态 →
// 取 CItemLimitEdition（getItemLimitEditionMgr→getItemInfo）→ 校验售罄/售价期/余额/限购条件 →
// CCeraShop::checkBuyingCondition → 限制道具处理（GetRestrictCode/IsRestrictedGoods/SetRestrictedGoods）→
// 更新购买时间 → 组 Packet_Item_Limit_Edition_Buyable_Query 发监控代理。
//
// 异常约定同 ORIG：业务校验失败 throw 4 字节 int（ENUM_ERROR 仅类型标签），try 外层 catch 后
// SendCmdErrorPacket(0x100, err) 并 return 0。
//
// _checkUserState / _checkBuyCondition 是该类独立 ORIG 成员（_ZNK…15_checkUserState /
// _ZNK…18_checkBuyCondition），已在 Dispatchers.h 私有区声明（const 成员），本 TU 定义产出 T 强符号。
// ============================================================================

// _checkUserState —— ORIG 0x0821f72a：CSecu Check(0x39)→IsBusyPrivateStore→CheckInTrade→
//   getCurSec()-lastBuyTime<0x1e→9。返回 0=通过，否则错误码。
// [N96 清绑 2026-09-09] 删除历史 `register bool isInInterval asm("al")`（P0 限定② /
// N96 全域禁用；P248 不得恢复）。报告 docs/class_func_reports/Dispatcher_BuyCeraShopLimitItem/
// _checkUserState.md 两段已读（Ghidra C 的 iVar2 - iVar3 < 0x1e ⇒ 我方三目条件）。
// ORIG 0x0821f7b2 `setle %al; test %al,%al; je`——isInInterval 值在 %al、无栈槽
// ⇒ 按 P0 双推律原码无该具名局部，还原为三目表达式直传。
// 残差 = N96+N18a 封顶（4.4 对整数比较恒直跳，不物化 setCC）。
// 读数：清绑前 [identical=6 ae=6 合计12 near=0 diff=2]，本函数 identical/ae；
//       清绑后 [identical=6 ae=5 合计11 near=0 diff=3]，verdict=DIFF
//       orig51/our49（缺 setle+test 两条）。
#include "Dispatcher_BuyCeraShopLimitItem_impl/_checkUserState.inc"
}

// _checkBuyCondition —— ORIG 0x0821f7cc：逐条件校验职业/等级/建号时间/账号注册时间区间，
//   任一不符返回 7，全过返回 0。
#include "Dispatcher_BuyCeraShopLimitItem_impl/_checkBuyCondition.inc"
}

// ============================================================================
// dispatch_sig
// ============================================================================
#include "Dispatcher_BuyCeraShopLimitItem_impl/dispatch_sig.inc"
}

#include "Dispatcher_BuyCeraShopLimitItem_impl/process.inc"
#include "Dispatcher_BuyCeraShopLimitItem_impl/read.inc"
