// ============================================================================
// [b28d-solo-CPrivateStoreMgr] 泳道判词（唯一写者：S413_CPrivateStoreMgr）
// 基线 identical=146 ae=289 near=1 diff=19 → 收口 identical=146 ae=294
// near=1 diff=14（diff 列表含 N123 噪音 _Z41__static_initialization...）。
// 本轮 20 件清单：完全修复 5 件（MoveToGate / RemovePrivateStore /
// SavePrivateStore / put_object_info / MoveItems，均为 ae——残差仅
// __PRETTY_FUNCTION__/memset@plt 重定位显示形，AE 等价）；部分修复 6 件
// （CreatePrivateStore / DeleteItem / RecallPrivateStoreItem /
// RecallPrivateStoreItemToMail / IsAreaNearEntranceDungeon / AddItem）；
// 余 8 件为 N 族封顶（N82 帧槽 / N93 块内对象槽向 / N98 首次使用序 /
// N99 死比较 DCE / P75 实参装载序），逐件证据见各函数头注释与
// /tmp/b28d_solo_CPrivateStoreMgr_result.tsv。
// P167：清单内全部函数调用点差集 CLEAR（0 missing / 0 extra）。
// 无子子代理；本轮无 .h 改动落盘（CheckTotalItemPrice int→bool 已按 R2/S2
// 申报 Main 代落，依据 ORIG 0x085c73e2 `xor eax,0x1`，T3 直证）。
// ----------------------------------------------------------------------------
// [BigStore 巨型件轮] ndiff>30 前 8 件复核（基线 identical=171 ae=362 near=1
// diff=14 → 收口 identical=172 ae=362 near=2 diff=13）：
//  · REAL-BUG×3（BuyPrivateStoreItem/VerifyPrivateStoreItem）：
//    IsAvailableCurCharacTradeGoldDaily 实参两处传 dealGoldA（outBuyGold 恒 0）
//    ⇒ 非可信用户每日金币上限校验被短路；ORIG 0x085c8186/0x085c8ae8 实证传
//    GetItemPrice out1(-0x44/-0x28=price)。BuyPSI 反外挂倾销公式两处同错
//    （0x085c8526/0x085c8558 用 price，旧传恒 0 ⇒ 检测整体失效），并按 ORIG
//    x87 栈形补 (float) 窄化（0x085c8542 fstps/flds）。BuyItem 第 5 实参
//    改 out3（ORIG 0x085c8261 段，行为无差、忠实性）。
//  · 语义重构 2 件：DeleteItem 340→306（switch 跳转表→ORIG if-else 有符号链 +
//    每臂延迟重读 +0x48）；RecallPrivateStoreItem 375→188 转 near/N82（i 改
//    int+双初始化+第二循环复用 i 槽+==3/==4 正向包裹+两 invenW 具名缓存删）。
//  · P329 去具名提升 2 件：CreatePrivateStore 214→90（inventory 链式内联 +
//    dollError P225 while 形）、BuyPSI 363→322。
//  · 封顶（残差=已登记 N 族，不再抠旋钮）：Recall 188=N82 槽镜像；BuyPSI
//    322=N85(x87 fucompp)+N82(帧 0xbc)；DeleteItem 306=N82+P227 trace 位；
//    BuyItem 110=N18a 寄存器直落+N82；Create 90=N82 槽漂移；Verify 77=N82；
//    MakeCmdpacketEnter 58=N89(STL out-of-line, ORIG W 0x085cc160)；IsAreaNear
//    50=N111(0x085c5273..0x085c5299 无入口死块)+N82。
//  P167 本批 8 件：除 IsAreaNear(已豁免)外全 CLEAR。头文件未动（CPrivateStoreMgr.h
//  归 CMisVtable 泳道，虚槽保持现状）。
// ============================================================================
#include "CPrivateStoreMgr.h"
#include "CErrorGeneratorPrivate.h"   // private_store::CErrorGeneratorPrivate / g_ErrorGenerator
#include "MoreTypes.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CUser.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString（RecallToMail 文案）
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail
#include "AuctionProxy.h"             // CAuctionServerProxy::IsRunning/SendPacket
#include "PacketDesign.h"             // nsl::PACKET_HEADER / PACKET_CTOR_BODY
#include "Village.h"                  // Village::get_gate_area / getArea（Area 字段）
#include "CStackableItem.h"           // CheckPrivateStoreDoll 的 +0x32c 字段
#include "CInventory.h"              // CInventory 完整类型（GetInvenSlot/use_money/gain_money 等）
#include "AvatarItem.h"              // 全局 Avatar_Item（BuyItem 所有权转移链）
#include "CAvatarItemMgr.h"           // WongWork::CAvatarItemMgr（avatar 剩余期/宝石槽/扩展）
#include "CCirculationStatistic.h"   // GetInstanceCirculationStatistic/CIRCULATION_STATISTIC_FIELD
#include "HistoryLog.h"               // HistoryLog::WriteBuyItem/WriteSellItem
#include "DB_AvatarChangeOwner.h"     // avatar 过户 DB 请求
#include "DisPatcherCore.h"           // DisPatcher_LeaveParty（CreatePrivateStore 离队）
#include "PCK_AUCTION_OPEN_PRIVATE_STORE_GA.h"  // 权威 PCK_AUCTION_OPEN_PRIVATE_STORE_GA（ORIG W 0x85cb374）
#include "PCK_AUCTION_CLOSE_PRIVATE_STORE_GA.h" // 权威 PCK_AUCTION_CLOSE_PRIVATE_STORE_GA（ORIG W 0x85cb3dc）


#include <algorithm>
#include <cstring>
#include <cerrno>   // IsAreaNearEntranceDungeon 异常垫（ORIG strerror(errno)）
// ORIG T _Z26IsTradeLimitAttachTypeItemRK10Inven_Item @0x081201dc（定义在 CMailBoxHelper TU）。
bool IsTradeLimitAttachTypeItem(Inven_Item const& item);

// PCK_AUCTION_OPEN/CLOSE_PRIVATE_STORE_GA 均归一化到权威共享头（eliminate 伪镜像
// OpenPrivateStoreGaMirror / ClosePrivateStoreGaMirror，统一 ORIG 符号
// /_ZN33PCK_AUCTION_OPEN_PRIVATE_STORE_GAC1Ev/、/_ZN34PCK_AUCTION_CLOSE_PRIVATE_STORE_GAC1Ev/）。

namespace private_store
{
#include "CPrivateStoreMgr/GetInstancePrivateStoreMgr.inc"

#include "CPrivateStoreMgr/COperator_Product.inc"


#include "CPrivateStoreMgr/CPrivateStoreMgr_ctor.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_dtor.inc"

// ---- PrivateStoreZone / PrivateStorePos（定义在 TU 尾 inline 镜像区；
//      ORIG C1/C2(h,h) W 0x85cb6ce、Pos C1 W 0x85cb6f6 / 0x85cb72c、
//      clear W 0x85cb784。后置定义复现 ORIG 调用点 conservative outgoing）----

// ---- PrivateStoreItem（定义在 TU 尾 inline 镜像区；ORIG C1 0x823457c /
//      Reset 0x823459a / op= 0x85cb66e）----

// ---- CPrivateStorePos（定义在 TU 尾 inline 镜像区；
//      ORIG C1 0x85cb7c8 / 带参 C1 0x85cb7fc）----

#include "CPrivateStoreMgr/CPrivateStorePos_put_object_info.inc"

#include "CPrivateStoreMgr/CPrivateStorePos_include.inc"

#include "CPrivateStoreMgr/CPrivateStorePos_operator_assign.inc"

#include "CPrivateStoreMgr/CPrivateStore_ctor.inc"

#include "CPrivateStoreMgr/CPrivateStore_dtor.inc"

#include "CPrivateStoreMgr/CPrivateStore_isDollCreated.inc"

#include "CPrivateStoreMgr/CPrivateStore_GetSellerIndex.inc"

#include "CPrivateStoreMgr/CPrivateStore_ResetItems.inc"

#include "CPrivateStoreMgr/CPrivateStore_IsLimitedSpace.inc"

#include "CPrivateStoreMgr/CPrivateStore_CopyItems.inc"

#include "CPrivateStoreMgr/CPrivateStore_Clear.inc"

#include "CPrivateStoreMgr/CPrivateStore_RecallPrivateStoreItemToMail.inc"

#include "CPrivateStoreMgr/CPrivateStore_SavePrivateStore.inc"
#include "CPrivateStoreMgr/CPrivateStore_RecreatePrivateStore.inc"

#include "CPrivateStoreMgr/CPrivateStore_CreatePrivateStore.inc"

#include "CPrivateStoreMgr/CPrivateStore_RemovePrivateStore.inc"

#include "CPrivateStoreMgr/CPrivateStore_CompleteDisplay.inc"

#include "CPrivateStoreMgr/CPrivateStore_MoveToGate.inc"

#include "CPrivateStoreMgr/CPrivateStore_EnterPrivateStore.inc"

#include "CPrivateStoreMgr/CPrivateStore_ExitPrivateStore.inc"

#include "CPrivateStoreMgr/CPrivateStore_AddItem.inc"

#include "CPrivateStoreMgr/CPrivateStore_BuyPrivateStoreItem.inc"

#include "CPrivateStoreMgr/CPrivateStore_VerifyPrivateStoreItem.inc"

#include "CPrivateStoreMgr/CPrivateStore_CheckPrivateStoreDoll.inc"

#include "CPrivateStoreMgr/CPrivateStore_IsAreaNearEntranceDungeon.inc"

#include "CPrivateStoreMgr/CPrivateStore_MoveItems.inc"

#include "CPrivateStoreMgr/CPrivateStore_RecallPrivateStoreItem.inc"

#include "CPrivateStoreMgr/CPrivateStore_DeleteItem.inc"

#include "CPrivateStoreMgr/CPrivateStore_GetItemInfo.inc"

#include "CPrivateStoreMgr/CPrivateStore_SendUsers.inc"

#include "CPrivateStoreMgr/CPrivateStore_IsExistBuyerList.inc"

// ---- CheckValidItem 三重载（ORIG T 0x85c9d4e / 0x85c9bb8 / 0x85c9cc2，三份报告全文）----

#include "CPrivateStoreMgr/CPrivateStore_CheckValidItem_item.inc"

#include "CPrivateStoreMgr/CPrivateStore_CheckValidItem_type_slot_count.inc"

#include "CPrivateStoreMgr/CPrivateStore_CheckValidItem_cargoSlot.inc"

#include "CPrivateStoreMgr/CErrorHandler_Set.inc"

#include "CPrivateStoreMgr/CErrorHandler_Do.inc"

#include "CPrivateStoreMgr/CPrivateStore_CheckTotalItemPrice.inc"

#include "CPrivateStoreMgr/CPrivateStore_IsPossibleDeal.inc"

#include "CPrivateStoreMgr/CPrivateStore_SubtractItemCount.inc"

#include "CPrivateStoreMgr/CPrivateStore_GetItemPrice.inc"

#include "CPrivateStoreMgr/CPrivateStore_GetCommissionPercent.inc"

#include "CPrivateStoreMgr/CPrivateStore_BuyItem.inc"

#include "CPrivateStoreMgr/CPrivateStore_SellItem.inc"


#include "CPrivateStoreMgr/CBroadcastPrivate_Send.inc"

#include "CPrivateStoreMgr/CCharacterControllerPrivate_MoveCharacterStartPoint.inc"

// ORIG B _ZN13private_store21g_CharacterControllerE @0x09472de0（BSS size 6）
CCharacterControllerPrivate g_CharacterController;

#include "CPrivateStoreMgr/CErrorGeneratorPrivate_ctor.inc"

#include "CPrivateStoreMgr/CErrorGeneratorPrivate_dtor.inc"

// ORIG B _ZN13private_store16g_ErrorGeneratorE @0x09472de8（BSS size 0x18）；
// 全局构造序列（_GLOBAL__I__...GetInstancePrivateStoreMgr）实证：C1 构造 + __cxa_atexit(D1)。
CErrorGeneratorPrivate g_ErrorGenerator;

#include "CPrivateStoreMgr/CCharacterControllerPrivate_ctor.inc"

#include "CPrivateStoreMgr/CCharacterControllerPrivate_GetStartPoint.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeErrorReturnPacket.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_FindStoreSeller.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_FindStoreBuyer.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_FreeStoreBuyer.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_FreeStoreSeller.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_GetStoreSeller.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_GetStoreBuyer_direct.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_GetStoreBuyer_byIndex.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_Enter.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_Exit.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_HandleError.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_IsBusyPrivateStore.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_LoadPrivateStore.inc"

// ---- MakeNotipacketPrivateStoreInfo（ORIG W 0x85cc5bc，objdump 逐指令转写）----
// 通知包：header(0,0x55)、byte(sig+4)、byte(占位计数)后遍历 14 条目，
// state==3 者写 short(槽)/int(售价 +0x40)/int(item idx)/int(add_info2)/
// byte(GetItemAttr)/short(endurance)/int(+0xd)/byte+short(强化类型与值)/put_packet(item)，
// 回填计数占位并 finalize(true)。返回上柜条数。
// [b28d-24] DIFF 残54: 协议字段序列化槽序/顺序（P95/P99）。ORIG 0x085cc5bc
// [b28d-solo-CPrivateStoreMgr] 残 39 行：P311 成员数组视图（PrivateStoreItemsView
// 的 `m_items[14]` 成员数组）把寻址链从 6 条/处降到 ORIG 的 4 条形（102→39）；
// N93 标量例外调 i/count 槽序（i 零初始化提前）。余为寄存器角色互换 + 一处
// lea-vs-add（N7/N1）。P292 档① `== 3` 正向包裹已应用。
// [b28d-solo-CPrivateStoreMgr] P311 成员数组视图：ORIG 0x085cc5bc 每条条目访问
// 发 `mov i; imul $0x4c; add sig; add $8`（索引先算，4 条）；字节数组 + cast
// 形 4.4 恒基址先算（6 条）。用具名数组成员视图替代裸字节 cast。
// [L14] P235：改用权威头 SIG_PRIVATE_STORE 的**具名类型数组视角** m_storeItems[]
// （MoreTypes.h 匿名 union 双视角，POD packed，sizeof 0x4c 断言）：字段读走
// `sig->m_storeItems[i].m_addInfo` ⇒ 4.4 折 scaled disp（ORIG 0x85cc5bc 段
// `mov 0xa/0xf/0x13/0x15(%edx,%eax,1)`）；裸字节数组 + cast 形恒先物化 base
// （多 1 条 `add $8`）。需真实类型（方法调用/引用实参）处用 PS_REAL。
// [L14 结论] 156/152 → 153/152（gap +4→+1）：4 处字段读全等；m_addInfo(0xa)/
// m_fieldd(0x15) 经镜像 **enum 视角**复现 ORIG 的 eax→edx 两段装载（P254，
// m_addInfo2(0xf) 保持 int 单段）。余 2 处 = ORIG `add $0x10;…;add $0x9` 加法
// 分组 vs 我方 `add $8; add $0x11`（同 4 条，N1 常量分组）+ put_packet 实参
// ORIG `lea 0x8(%eax),%edx` vs 我方 `add $8,%eax; mov %eax,%edx`（N107：成员常量
// 偏移取址 4.4 恒 add、4.1 恒 lea，六形探针全败）⇒ 停 NEAR 级残差。
#define PS_ITEM(i) (sig->m_storeItems[(i)])
#define PS_REAL(i) (*reinterpret_cast<PrivateStoreItem*>(&sig->m_storeItems[(i)]))
#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketPrivateStoreInfo.inc"
#undef PS_ITEM

// ---- 9 槽全链包构造（ORIG W 内联符号，objdump 逐指令转写，PsBuildB 交付）----

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketCreatePrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketCreatePrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketMoveToGate.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketRemovePrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketRemovePrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketForceRemovePrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketExitPrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketEnterPrivateStore.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketCompleteDisplay.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketCompleteDisplay.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketPrivateStoreItem.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeNotipacketBuyPrivateStoreItem.inc"

#include "CPrivateStoreMgr/CPacketBufPrivate_MakeCmdpacketBuyPrivateStoreItem.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_LoadPrivateStoreResponse.inc"

#include "CPrivateStoreMgr/CPrivateStoreMgr_GetPrivateStoreSize.inc"
// ============================================================================
// ORIG W（头内联）镜像区：以下定义置于 TU 尾——本 TU 内更早的调用点按
// conservative outgoing 预留（sub $0x18），与 ORIG 产物一致；inline 标记
// 使符号为 W，与 ORIG nm 一致。
// ============================================================================

#include "CPrivateStoreMgr/PrivateStoreZone_ctor.inc"

#include "CPrivateStoreMgr/PrivateStorePos_ctor.inc"

#include "CPrivateStoreMgr/PrivateStorePos_ctor_abff.inc"

#include "CPrivateStoreMgr/PrivateStorePos_clear.inc"

#include "CPrivateStoreMgr/PrivateStoreItem_ctor.inc"

#include "CPrivateStoreMgr/PrivateStoreItem_Reset.inc"

#include "CPrivateStoreMgr/PrivateStoreItem_operator_assign.inc"

#include "CPrivateStoreMgr/CPrivateStorePos_ctor.inc"

#include "CPrivateStoreMgr/CPrivateStorePos_ctor_store.inc"

}

// ---- 暂落定义（三处 ORIG 真实符号，权威归属批次后续收编；本批唯一落点 CPrivateStoreMgr.cpp）----

#include "CPrivateStoreMgr/CVillageObjectMgr_unregister_object.inc"

#include "CPrivateStoreMgr/CItem_GetCashPrice.inc"

#include "CPrivateStoreMgr/STExpertJobScript_GetShopDollSlot.inc"
