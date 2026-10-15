// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPrivateStoreMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "CPrivateStoreMgr/IsTradeLimitAttachTypeItem.inc"

// PCK_AUCTION_OPEN/CLOSE_PRIVATE_STORE_GA 均归一化到权威共享头（eliminate 伪镜像
// OpenPrivateStoreGaMirror / ClosePrivateStoreGaMirror，统一 ORIG 符号
// /_ZN33PCK_AUCTION_OPEN_PRIVATE_STORE_GAC1Ev/、/_ZN34PCK_AUCTION_CLOSE_PRIVATE_STORE_GAC1Ev/）。

namespace private_store
{
// ORIG 0x085c4be8：函数内静态单例（__cxa_guard）。
CPrivateStoreMgr* GetInstancePrivateStoreMgr()
{
    static CPrivateStoreMgr instance;
    return &instance;
}

// ---- COperator<int>::Product 特化（ORIG W _ZN13private_store9COperatorIiE7ProductEiiRi @0x85cd22a）----
// 重复加法乘法：out 累加 a 共 b 次，任一步非正即失败返回 0。须先于首次隐式实例化。
template <>
__attribute__((weak)) bool COperator<int>::Product(int a, int b, int& out)
{
    out = 0;
    for (int i = 0; i < b; ++i)
    {
        out = out + a;
        if (!(out > 0))
            return false;
    }
    return true;
}


#include "CPrivateStoreMgr/CPrivateStoreMgr.inc"


#include "CPrivateStoreMgr/dtor_CPrivateStoreMgr.inc"


// ---- PrivateStoreZone / PrivateStorePos（定义在 TU 尾 inline 镜像区；
//      ORIG C1/C2(h,h) W 0x85cb6ce、Pos C1 W 0x85cb6f6 / 0x85cb72c、
//      clear W 0x85cb784。后置定义复现 ORIG 调用点 conservative outgoing）----

// ---- PrivateStoreItem（定义在 TU 尾 inline 镜像区；ORIG C1 0x823457c /
//      Reset 0x823459a / op= 0x85cb66e）----

// ---- CPrivateStorePos（定义在 TU 尾 inline 镜像区；
//      ORIG C1 0x85cb7c8 / 带参 C1 0x85cb7fc）----

#include "CPrivateStoreMgr/put_object_info.inc"


// ORIG W 0x85cb85c：对双方 +0x04 处的 Rectangle 做 include 判定
// （村庄对象矩形统一位于 IObject 头之后，与 ORIG lea 0x4(%eax) 一致）。
__attribute__((weak)) bool CPrivateStorePos::include(village_object::IObject const& obj) const
{
    // ORIG：无具名中间量（不 spill），(obj+4) 先求值入 %eax、lea 0x4(%this)
    // 入 %edx 后直落参数槽 ⇒ 表达式内联。
    return m_rect.include(
        *reinterpret_cast<collision_detection::Rectangle const*>(
            reinterpret_cast<char const*>(&obj) + sizeof(village_object::IObject)));
}

// ORIG W 0x85ccae4：基类子对象赋值 + Rectangle 4 个 int 直拷 + 宿主指针。
__attribute__((weak)) CPrivateStorePos& CPrivateStorePos::operator=(CPrivateStorePos const& other)
{
    village_object::IObject::operator=(other);  // ORIG W 0x85ccadc：空操作返回自身
    // ORIG 0x85ccae4：单基址四 int 顺序拷贝（mov 0x4/8/c/10(%edx),%ecx → %eax）
    // ⇒ Rectangle 结构体整体赋值（POD 隐式逐字拷贝），非逐字段（逐字段每行双 reload）。
    m_rect = other.m_rect;
    m_pStore = other.m_pStore;
    return *this;
}

#include "CPrivateStoreMgr/CPrivateStore.inc"


#include "CPrivateStoreMgr/dtor_CPrivateStore.inc"


// ORIG W 0x85cb894：卖家 world unique id（GetSeller()->get_unique_id()）。
__attribute__((weak)) unsigned short CPrivateStore::GetSellerIndex()
{
    return m_pSeller->get_unique_id();
}

#include "CPrivateStoreMgr/ResetItems.inc"


#include "CPrivateStoreMgr/IsLimitedSpace.inc"


#include "CPrivateStoreMgr/CopyItems.inc"


#include "CPrivateStoreMgr/Clear.inc"


#include "CPrivateStoreMgr/RecallPrivateStoreItemToMail.inc"


#include "CPrivateStoreMgr/SavePrivateStore.inc"

#include "CPrivateStoreMgr/RecreatePrivateStore.inc"


#include "CPrivateStoreMgr/CreatePrivateStore.inc"


#include "CPrivateStoreMgr/RemovePrivateStore.inc"


#include "CPrivateStoreMgr/CompleteDisplay.inc"


#include "CPrivateStoreMgr/MoveToGate.inc"


#include "CPrivateStoreMgr/EnterPrivateStore.inc"


#include "CPrivateStoreMgr/ExitPrivateStore.inc"


#include "CPrivateStoreMgr/AddItem.inc"


#include "CPrivateStoreMgr/BuyPrivateStoreItem.inc"


#include "CPrivateStoreMgr/VerifyPrivateStoreItem.inc"


#include "CPrivateStoreMgr/CheckPrivateStoreDoll.inc"


#include "CPrivateStoreMgr/IsAreaNearEntranceDungeon.inc"


#include "CPrivateStoreMgr/MoveItems.inc"


#include "CPrivateStoreMgr/RecallPrivateStoreItem.inc"


#include "CPrivateStoreMgr/DeleteItem.inc"


#include "CPrivateStoreMgr/GetItemInfo.inc"


#include "CPrivateStoreMgr/SendUsers.inc"


#include "CPrivateStoreMgr/IsExistBuyerList.inc"


// ---- CheckValidItem 三重载（ORIG T 0x85c9d4e / 0x85c9bb8 / 0x85c9cc2，三份报告全文）----

#include "CPrivateStoreMgr/CheckValidItem.inc"


#include "CPrivateStoreMgr/CheckValidItem_1.inc"


#include "CPrivateStoreMgr/CheckValidItem_2.inc"


// ---- CErrorHandler::Set（ORIG W 0x85cb7a8，objdump 逐指令：三条直存无副作用）----
__attribute__((weak)) void CErrorHandler::Set(CUser* user, int sendError, int code)
{
    m_user = user;
    m_sendError = sendError;
    m_code = code;
}

#include "CPrivateStoreMgr/Do.inc"


#include "CPrivateStoreMgr/CheckTotalItemPrice.inc"


#include "CPrivateStoreMgr/IsPossibleDeal.inc"


#include "CPrivateStoreMgr/SubtractItemCount.inc"


#include "CPrivateStoreMgr/GetItemPrice.inc"


#include "CPrivateStoreMgr/GetCommissionPercent.inc"


#include "CPrivateStoreMgr/BuyItem.inc"


#include "CPrivateStoreMgr/SellItem.inc"



// ---- CBroadcastPrivate::Send（ORIG W 0x85cca73，objdump 逐指令转写）----
// vill/area 双零直接返回；否则交 GameWorld 按村庄+区域广播。
__attribute__((weak)) void CBroadcastPrivate::Send(int vill, int area, PacketGuard& packet)
{
    if (vill == 0 && area == 0)
        return;
    G_GameWorld()->send_to_area(vill, area, packet);   // ORIG 0x86c8168
}

// ---- CCharacterControllerPrivate::MoveCharacterStartPoint（ORIG W 0x85cc9d2，objdump 逐指令）----
// 以 this 充当起点 Pos 重算后 move_area 回城门（z/flag/prev 全 0，返回值忽略）。
__attribute__((weak)) void CCharacterControllerPrivate::MoveCharacterStartPoint(CUser* user)
{
    GetStartPoint(user, m_startPos);
    G_GameWorld()->move_area(user,
                             (int)m_startPos.m_a,
                             (int)m_startPos.m_b,
                             (int)(short)m_startPos.m_field2,
                             (int)(short)m_startPos.m_field4,
                             0, false, 0, 0, 0);       // ORIG move_area 0x86c5a84
}

// ORIG B _ZN13private_store21g_CharacterControllerE @0x09472de0（BSS size 6）
CCharacterControllerPrivate g_CharacterController;

// ---- CErrorGeneratorPrivate（ORIG C1 W 0x85d0fa4 / D1 W 0x85d0f7c，报告缺失，objdump 转写）----
// 唯一成员 std::map<CUser*,int>：C1 调 map C1、D1 调 map D1（sizeof 0x18）。
__attribute__((weak)) CErrorGeneratorPrivate::CErrorGeneratorPrivate() : m_errors() {}

__attribute__((weak)) CErrorGeneratorPrivate::~CErrorGeneratorPrivate() {}

// ORIG B _ZN13private_store16g_ErrorGeneratorE @0x09472de8（BSS size 0x18）；
// 全局构造序列（_GLOBAL__I__...GetInstancePrivateStoreMgr）实证：C1 构造 + __cxa_atexit(D1)。
CErrorGeneratorPrivate g_ErrorGenerator;

// ---- CCharacterControllerPrivate（ORIG C1 W 0x85d0f90）----
__attribute__((weak)) CCharacterControllerPrivate::CCharacterControllerPrivate() throw() : m_startPos() {}

// ---- CCharacterControllerPrivate::GetStartPoint（ORIG W 0x85cc894）----
// 计算私商开店起始坐标：pos 先清零再写 village 号(a)；user/world/village 缺失
// 时仅 trace 并保持 b=0。注意 ORIG 后两个失败分支复用 "G_GameWorld call failed"
// 文案（0x8cbd0cf，原版即如此），逐字保留。
// 【b28 P225】world/village 判空的 `cmp $0; sete %al; test; je` 物化岛改
// `while (p == 0) { trace; break; }` 形复现（P225 修正 P197④「槽比恒不可翻」
// 边界；旧 if-return 形产 cmp/jne 直跳为残差主因）。
__attribute__((weak)) void CCharacterControllerPrivate::GetStartPoint(CUser* user, PrivateStorePos& pos)
{
    pos.m_field2 = 0x172;
    pos.m_field4 = 0xbe;
    pos.m_a = 0;
    pos.m_b = 0;
    GameWorld* world = 0;    // P197③ ORIG movl $0x0,-0x10/-0xc 双槽预置（位于 pos 写后）
    Village* village = 0;
    if (user == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x183, 5)("pUser == NULL");
        return;
    }
    pos.m_a = (unsigned char)user->getCurCharacVill();
    world = G_GameWorld();
    while (world == 0)   // P225 while 四段物化复现 sete 岛；goto 尾保持原语义
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x18b, 5)("G_GameWorld call failed");
        goto out;
    }
    village = world->getUserVillage(user);
    while (village == 0)   // P225 同上
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x190, 5)("G_GameWorld call failed");
        goto out;
    }
    pos.m_b = (unsigned char)village->get_gate_area();
out:
    ;
}

// ---- CPacketBufPrivate::MakeErrorReturnPacket（ORIG W 0x85cc830）----
__attribute__((weak)) void CPacketBufPrivate::MakeErrorReturnPacket(int code1, int code2, int code3)
{
    clear();
    put_header(code1, code2);
    put_byte(0);
    put_byte(code3);
    finalize(true);
}

#include "CPrivateStoreMgr/FindStoreSeller.inc"


#include "CPrivateStoreMgr/FindStoreBuyer.inc"


#include "CPrivateStoreMgr/FreeStoreBuyer.inc"


#include "CPrivateStoreMgr/FreeStoreSeller.inc"


#include "CPrivateStoreMgr/GetStoreSeller.inc"


#include "CPrivateStoreMgr/GetStoreBuyer.inc"


#include "CPrivateStoreMgr/GetStoreBuyer_1.inc"


#include "CPrivateStoreMgr/Enter.inc"


#include "CPrivateStoreMgr/Exit.inc"


#include "CPrivateStoreMgr/HandleError.inc"


#include "CPrivateStoreMgr/IsBusyPrivateStore.inc"


#include "CPrivateStoreMgr/LoadPrivateStore.inc"


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
__attribute__((weak)) int CPacketBufPrivate::MakeNotipacketPrivateStoreInfo(SIG_PRIVATE_STORE* sig)
{
    clear();
    put_header(0, 0x55);
    put_byte((int)sig->m_field04);
    int countIndex = get_index();
    put_byte(0);

    // [L14] 判据⑩（槽序反转律）：ORIG 槽 i@-0x10（深）/count@-0xc（浅）⇒ i 声明在
    // count 之前（先声明=更深槽）；而 ORIG 初始化指令序为 count(-0xc) 先、i(-0x10)
    // 后 ⇒ i 声明与初始化分离（`int i;` + for-init）。
    int i;
    int count = 0;
    for (i = 0; i <= 0xd; ++i)
    {
        if (PS_REAL(i).GetState() == 3)
        {
            put_short(i);
            put_int(PS_ITEM(i).m_field40);            // 售价（条目 +0x40）
            put_int(PS_ITEM(i).m_addInfo);            // 物品索引（条目 +0x02）
            put_int(PS_ITEM(i).m_addInfo2);           // 条目 +0x07
            put_byte((int)PS_REAL(i).m_item.GetItemAttr());
            put_short((int)PS_ITEM(i).m_endurance);      // 耐久（条目 +0x0b）
            put_int((int)PS_ITEM(i).m_extendInfo);        // 条目 +0x0d
            put_byte((int)PS_REAL(i).m_item.m_amp.getAbilityType());
            put_short((int)PS_REAL(i).m_item.m_amp.getAbilityValue());
            // [L14] ORIG 该实参为 `imul; add sig; lea 0x8(%eax),%edx`（元素取址）；
            // cast 到 Inven_Item* 直取元素首地址（m_item 在 PrivateStoreItem +0）。
            put_packet(*reinterpret_cast<Inven_Item const*>(&sig->m_storeItems[i]));
            ++count;
        }
    }

    put_byte(countIndex, count);
    finalize(true);
    return count;
}
#undef PS_ITEM

// ---- 9 槽全链包构造（ORIG W 内联符号，objdump 逐指令转写，PsBuildB 交付）----

// ORIG _ZN13private_store17CPacketBufPrivate31MakeCmdpacketCreatePrivateStoreEv @0x85cb986
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketCreatePrivateStore()
{
    clear();
    put_header(1, 0x58);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate32MakeNotipacketCreatePrivateStoreEtRNS_15PrivateStorePosEb @0x85cb9da
__attribute__((weak)) void CPacketBufPrivate::MakeNotipacketCreatePrivateStore(unsigned short sellerIndex,
                                                         PrivateStorePos& pos, bool dollCreated)
{
    clear();
    put_header(0, 0x4e);
    put_short((int)sellerIndex);
    put_byte((int)pos.m_a);
    put_byte((int)pos.m_b);
    put_short((int)(short)pos.m_field2);
    put_short((int)(short)pos.m_field4);
    if (dollCreated)
        put_byte(1);
    else if (!dollCreated)
        put_byte(0);   // ORIG xor $1 二次测试的字面结构（净语义等价 else）
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate23MakeCmdpacketMoveToGateEv @0x85cc7dc
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketMoveToGate()
{
    clear();
    put_header(1, 0x5b);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate32MakeNotipacketRemovePrivateStoreEt @0x85cbb2c
__attribute__((weak)) void CPacketBufPrivate::MakeNotipacketRemovePrivateStore(unsigned short sellerIndex)
{
    clear();
    put_header(0, 0x4f);
    put_short((int)sellerIndex);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate31MakeCmdpacketRemovePrivateStoreEv @0x85cbad8
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketRemovePrivateStore()
{
    clear();
    put_header(1, 0x59);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate37MakeNotipacketForceRemovePrivateStoreEv @0x85cbb88
__attribute__((weak)) void CPacketBufPrivate::MakeNotipacketForceRemovePrivateStore()
{
    clear();
    put_header(0, 0x50);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate29MakeCmdpacketExitPrivateStoreEv @0x85cc568（包仅发买家）
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketExitPrivateStore()
{
    clear();
    put_header(1, 0x57);
    put_byte(1);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate30MakeCmdpacketEnterPrivateStoreEP5CUsertRSt6vectorINS_16PrivateStoreItemESaIS4_EE @0x85cc160
// 仅序列化 GetState()==3 的条目；条目数先写占位字节、循环后回填。
// [b28d-24] DIFF 残32: 计数器局部槽/循环形状（P25/P31）。ORIG 0x085cc160
// [b28d-solo-CPrivateStoreMgr] 已修条件极性（`==3` 正向包裹，P292 档①）；
// 残 56 行 = nowTick/writtenCount/i 槽序（N98，4.4 按首次使用序分配；声明序互换
// 实测 66 劣化 ⇒ 封顶）+ 寻址链（N1/P30）+ put_binary 实参装载序（P75）。
// [L14] 本轮复核：277/276，唯一差异块（4 行）= 同族 N7：ORIG `mov %eax,%edx;
// mov 0x8(%ebp),%eax`（值先入 %edx 中转）vs 我方 `mov 0x8(%ebp),%edx`（missing 中转）。
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketEnterPrivateStore(CUser* sellerUser,
                                                       unsigned short sellerIndex,
                                                       std::vector<PrivateStoreItem>& items)
{
    clear();
    put_header(1, 0x56);
    put_byte(1);
    put_short((int)sellerIndex);

    // [b28d-PS] P191 双初始化 + 槽序：ORIG 0x085cc1d6 在 get_index 之前发
    // `movl $0x0,-0x18`（i 的声明初始化），循环前 0x085cc526 再发一次 ⇒
    // `int i = 0;` 必须声明在 countIndex 之前、for 用 `i = 0` 赋值。
    int i = 0;
    int countIndex = get_index();               // ORIG 0x8110b4c
    put_byte(0);
    int nowTick = OS_API::GetDateTimeTick();
    int writtenCount = 0;
    for (i = 0; i <= 13; ++i)
    {
        // P292 档①：ORIG `cmp $3,%al; sete %al; test; je` ⇒ 正向 ==3 包裹
        //（`!=3 continue` 会产 setne/jne 反向形）。
        if (items[i].GetState() == 3)
        {
        put_short(i);
        put_int(items[i].m_field40);                              // +0x40 售价
        put_int((int)items[i].m_item.m_addInfo);                  // +0x02 物品索引
        if ((int)items[i].m_item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
        {
            // avatar：剩余时间按 uid 实时查询（tick 在循环外取一次）。
            put_int(sellerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                        (int)items[i].m_item.m_addInfo2, nowTick));
        }
        else
        {
            put_int((int)items[i].m_item.m_addInfo2);             // +0x07
        }
        put_byte((int)items[i].m_item.GetItemAttr());
        put_short((int)items[i].m_item.m_endurance);                 // +0x0b 耐久
        put_int((int)items[i].m_item.m_extendInfo);                   // +0x0d
        put_byte((int)items[i].m_item.m_amp.getAbilityType());
        put_short((int)items[i].m_item.m_amp.getAbilityValue());
        if ((int)items[i].m_item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
        {
            put_int(0x1e);
            // [PoolE 2026-09-11 负结论] (void*) 中间转换旋钮实测无效（ndiff 2 逐值不变）；
            // ORIG「实参值先入 %edx、this 后载 %eax」装载序属 N7/P75 跨版本寄存器角色族。
            put_binary((char const*)sellerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                           (int)items[i].m_item.m_addInfo2), 0x1e);
            put_int(4);
            void* expansion =
                sellerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(
                    (int)items[i].m_item.m_addInfo2);
            if (expansion != 0)
            {
                put_binary((char const*)expansion, 4);
            }
            else
            {
                stAvatarExpansionInfo_t defaultExpansion;
                defaultExpansion.init();
                put_binary((char const*)&defaultExpansion, 4);
            }
        }
        put_packet(items[i].m_item);          // ORIG put_packetERK10Inven_Item（const&）
        ++writtenCount;
        }
    }
    put_byte(countIndex, writtenCount);       // 回填占位字节（put_byteERii 0x8110b28）
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate29MakeNotipacketCompleteDisplayEtRSsb @0x85cbc52
__attribute__((weak)) void CPacketBufPrivate::MakeNotipacketCompleteDisplay(unsigned short sellerIndex,
                                                      std::string& subject,
                                                      bool dollCreated)
{
    clear();
    put_header(0, 0x51);
    put_short((int)sellerIndex);
    put_int((int)subject.length());
    put_str(subject.c_str(), (int)subject.length());
    // ORIG 0x85cbce6-0x85cbd1a：首 if 对 bool 槽 cmpb 直判走 put_byte(1)
    // （块尾 jmp 跳过 else 块）；else if(!p) 段 movzbl+xor$1+test 走
    // put_byte(0) —— else-if 链形（非 if/else 直配，否则无二次测试段）。
    if (dollCreated)
        put_byte(1);
    else if (!dollCreated)
        put_byte(0);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate28MakeCmdpacketCompleteDisplayEb @0x85cbbca
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketCompleteDisplay(bool dollCreated)
{
    clear();
    put_header(1, 0x5a);
    put_byte(1);
    if (dollCreated)
        put_byte(1);
    else
        put_byte(0);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate30MakeNotipacketPrivateStoreItemEii @0x85cc0fa（SendUsers 广播）
__attribute__((weak)) void CPacketBufPrivate::MakeNotipacketPrivateStoreItem(int slot, int count)
{
    clear();
    put_header(0, 0x53);
    put_short(slot);
    put_int(count);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate33MakeNotipacketBuyPrivateStoreItemEP5CUseriiiPKcii @0x85cbfea
// buyerUser=卖家 m_pSeller（收包人，序列化其金钱）；avatar 条目时 count 已由调用方换成 avatar uid。
__attribute__((weak)) void CPacketBufPrivate::MakeNotipacketBuyPrivateStoreItem(CUser* buyerUser, int itemId, int slot,
                                                          int count, char const* name,
                                                          int gold, int commission)
{
    clear();
    put_header(0, 0x52);
    put_byte(1);
    put_int(buyerUser->getCurCharacInvenR()->get_money());
    put_int(itemId);
    put_short(slot);
    put_int(count);
    put_int((int)std::strlen(name));
    put_str(name, (int)std::strlen(name));
    put_int(gold);
    put_int(commission);
    finalize(true);
}

// ORIG _ZN13private_store17CPacketBufPrivate32MakeCmdpacketBuyPrivateStoreItemEP5CUserR10Inven_Itemiiii @0x85cbd38
// buyerUser=买家 m_pBuyer（收包人，序列化金钱/胜点/cera）；a3/a4 在 ORIG 函数体内从未读取；
// a5=BuyItem 返回的落位槽位；a6=购买数量（avatar 时为 uid）。
// [b28d-solo-CPrivateStoreMgr] 残 21 行 = 3 处 `put_int/put_binary(链式调用结果)`
// 的实参装载序：ORIG `call; mov %eax,%edx; mov this,%eax; mov %edx,0x4(%esp);
// mov %eax,(%esp)`（实参先算入 edx），4.4 恒 `mov this,%edx; mov %eax,0x4(%esp);
// mov %edx,(%esp)`（this 先算）。receiver 为 packet this 固定，实参为链式调用
// 结果，无源码旋钮（P75 跨版本实参装载序）⇒ N 封顶；余为由此 1 条差引起的
// 4 处 jmp 偏移连锁。
// [L14] 本轮复核：185/184，唯一差异块 = ORIG `mov %eax,%edx; mov 0x8(%ebp),%eax;
// mov %edx,0x4(%esp); mov %eax,(%esp)` vs 我方 this→%edx/值→%eax ⇒ N7（长度全等）。
__attribute__((weak)) void CPacketBufPrivate::MakeCmdpacketBuyPrivateStoreItem(CUser* buyerUser, Inven_Item& item,
                                                         int a3, int a4, int a5, int a6)
{
    (void)a3;   // ORIG 未读参数占位
    (void)a4;
    clear();
    put_header(1, 0x55);
    put_byte(1);
    // ORIG `call 链; mov %eax,%edx; mov this,%eax; mov %edx,0x4(%esp);
    // mov %eax,(%esp)`（实参先于 this 装载）；P226 裸语句表达式实测劣化
    // （21→26）⇒ 保留直写，残差按 P75 跨版本实参装载序封顶。
    put_int(buyerUser->getCurCharacInvenR()->get_money());
    put_int(buyerUser->GetWinPoint());
    put_int(buyerUser->GetCera());
    put_short(a5);
    put_int((int)item.m_addInfo);
    int nowTick = OS_API::GetDateTimeTick();
    if ((int)item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
        put_int(buyerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(a6, nowTick));
    else
        put_int(a6);
    put_byte((int)item.GetItemAttr());
    put_short((int)item.m_endurance);
    put_int((int)item.m_extendInfo);
    put_byte((int)item.m_amp.getAbilityType());
    put_short((int)item.m_amp.getAbilityValue());
    if ((int)item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
    {
        put_int(0x1e);
        // [PoolE 2026-09-11 负结论] 同上（(void*) 旋钮无效，维持 P75/N7 封顶）。
        put_binary((char const*)buyerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(a6),
                   0x1e);
        put_int(4);
        void* expansion =
            buyerUser->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpansionInfo(a6);
        if (expansion != 0)
        {
            put_binary((char const*)expansion, 4);
        }
        else
        {
            stAvatarExpansionInfo_t defaultExpansion;
            defaultExpansion.init();
            put_binary((char const*)&defaultExpansion, 4);
        }
    }
    put_packet(item);
    finalize(true);
}

#include "CPrivateStoreMgr/LoadPrivateStoreResponse.inc"


// ORIG T 0x86d23a4（objdump 逐指令）：返回卖家表(+0x98) size()。
__attribute__((weak)) int CPrivateStoreMgr::GetPrivateStoreSize()
{
    return (int)m_storeSellers.size();
}
// ============================================================================
// ORIG W（头内联）镜像区：以下定义置于 TU 尾——本 TU 内更早的调用点按
// conservative outgoing 预留（sub $0x18），与 ORIG 产物一致；inline 标记
// 使符号为 W，与 ORIG nm 一致。
// ============================================================================

#include "CPrivateStoreMgr/PrivateStoreZone.inc"


#include "CPrivateStoreMgr/PrivateStorePos.inc"

#include "CPrivateStoreMgr/PrivateStorePos_1.inc"
                                        short field2, short field4)
    : PrivateStoreZone(a, b)
{
    m_field2 = field2;
    m_field4 = field4;
}

#include "CPrivateStoreMgr/clear.inc"
{
    m_a = 0;
    m_b = 0;
    m_field2 = 0;
    m_field4 = 0;
}

#include "CPrivateStoreMgr/PrivateStoreItem.inc"
{
    Reset();
}

#include "CPrivateStoreMgr/Reset.inc"
{
    m_item.reset();
    m_flag3d = 0;
    m_field40 = 0;
    m_field44 = 0;
    m_field46 = 0;
    m_field48 = 0;
}

// ORIG W 0x85cb66e：Inven_Item::setCopy 拷物品区，随后逐字段直拷并返回 *this。
inline PrivateStoreItem& PrivateStoreItem::operator=(PrivateStoreItem const& other)
{
    m_item.setCopy(other.m_item);
    m_flag3d = other.m_flag3d;
    m_field40 = other.m_field40;
    m_field44 = other.m_field44;
    m_field46 = other.m_field46;
    m_field48 = other.m_field48;
    return *this;
}

#include "CPrivateStoreMgr/CPrivateStorePos.inc"

#include "CPrivateStoreMgr/CPrivateStorePos_1.inc"


}

// ---- 暂落定义（三处 ORIG 真实符号，权威归属批次后续收编；本批唯一落点 CPrivateStoreMgr.cpp）----

#include "CPrivateStoreMgr/unregister_object.inc"


// ORIG W _ZNK5CItem12GetCashPriceEv @0x822c856：objdump 逐指令 mov 0x10(%eax) 直返。
__attribute__((weak)) int CItem::GetCashPrice() const
{
    return m_field10;   // +0x10
}

// ORIG W _ZN17STExpertJobScript15GetShopDollSlotEi @0x85cb306：objdump 逐指令转写。
// m_shopDollMap.find(index) 命中返 mapped 值，未命中哨兵 -1；唯一调用点 AddItem 0x85c6dfc。
__attribute__((weak)) short STExpertJobScript::GetShopDollSlot(int index)
{
    std::map<int, unsigned char>::iterator it = m_shopDollMap.find(index);
    if (it != m_shopDollMap.end())
        return (short)it->second;   // +0x04 mapped value（movzbl 零扩展；ORIG 用 operator->（pt））
    return -1;
}
