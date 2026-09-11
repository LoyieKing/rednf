#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CInventory/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r CInventory 还原（第八批 H3，2026-08-16）。
// 布局/语义以 docs/class_func_reports/CInventory.md 逐条对照（AE 口径）。
// 依赖类（Inven_Item/CItem/CUser/CAvatarItemMgr/CCreatureMgr 等）由
// H1/H2/后续批次提供，本 TU 仅声明引用。

#include "CInventory.h"

#include <stdlib.h>
#include <string.h>

#include "GameTypes.h"
#include "STItemScript.h"
#include "CCirculationStatistic.h"
#include "LogManager.h"
#include "CTitleBook.h"   // item_lock::CItemLock::MakeItemLockPacket（TSV）
#include "SigTypes.h"     // SIG_LOAD_*（GameWorld.h→CUser.h 依赖；须先于其 include）
#include "GameWorld.h"    // GameWorld::IsEnchantRevisionChannel/IsIntegratedPvPBaseChannel
#include "ServiceRestrictManager.h"   // ServiceRestrictManager::isRestricted（move_item 封印限制）
#include "Arad_DataManager.h"         // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "CSystemTime.h"              // CSystemTime::getCurSec（解封期限/头像剩余天数）
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper 静态邮件（insert_event_items 兜底）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（邮件标题/正文 key）
#include "CCubeStatistic.h"           // GetInstanceCubeStatistic/collectCubeStatistics（reason 8 统计）
#include <algorithm>                  // std::swap<int>/std::swap<INVEN_TYPE>

// GetIntegratedPvPItemAttr(3 参)/_getIntegratedPvPItemAttr 的自由函数（PvP 物品属性，
// ORIG 0x84fc651/0x84fc541）。1 参版本定义于 CDataManagerScripts.cpp，3 参版本见下方。
extern int _getIntegratedPvPItemAttr(const Inven_Item& item);

// 空槽占位 Inven_Item 全局（ORIG 0x943ddc0；定义于 CAccountCargo.cpp）
extern Inven_Item g_emptySlot;

#include "CInventory/isGainedGoldFromDungeonReason.inc"


#include "CInventory/write_log_gain_money.inc"


#include "CInventory/write_log_use_money.inc"



// ---- 发包依赖（StreamPool/CStreamGuard 经 GameBase.h 链已可见；MsgQueueMgr/GlobalData 本地声明） ----
class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2,
        TIMER_Q = 3,
        LOGDB_Q = 4
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;   // ORIG B 0x941f714（GlobalData.cpp 定义）
}
// ===================== 简单访问器（ORIG weak inline 形态） =====================


// 原 GetInvenSlotSelfHelper（ORIG 隐藏返回指针形态）已并入成员
// CInventory::GetInvenSlot（CInventory.h 返回 Inven_Item），删除替身。

#include "CInventory/GetCoin.inc"


#include "CInventory/SetCoin.inc"


#include "CInventory/GetPayCoin.inc"


#include "CInventory/SetPayCoin.inc"


#include "CInventory/GetEventCoin.inc"


#include "CInventory/SetEventCoin.inc"


#include "CInventory/getTotalCoin.inc"


#include "CInventory/IsCoinUsable.inc"


#include "CInventory/get_money.inc"


#include "CInventory/set_money.inc"


#include "CInventory/get_inventory_capacity.inc"


#include "CInventory/set_inventory_capacity.inc"


#include "CInventory/set_parent.inc"


#include "CInventory/SetInventoryMemory.inc"


#include "CInventory/GetCreatureMgrW.inc"


#include "CInventory/GetCreatureMgrR.inc"


#include "CInventory/GetAvatarItemMgrW.inc"


#include "CInventory/GetAvatarItemMgrR.inc"


#include "CInventory/init.inc"


#include "CInventory/GetClearAvatar.inc"


#include "CInventory/SetClearAvatar.inc"


#include "CInventory/GetClearAvatar_1.inc"


#include "CInventory/SetClearAvatar_1.inc"


#include "CInventory/_CompareSlot.inc"


#include "CInventory/CInventory.inc"
   // 三槽指针(+0x650/+0x654/+0x6e4)声明先于 AvatarCoin/CCreatureMgr 自动构造，故入 init-list(ORIG 序)

#include "CInventory/dtor_CInventory.inc"
{
}
// [b28d-round5 N84/P147 **翻正**，取代旧「N84 封顶」判词]
// ORIG `_ZN10CInventoryC1ERKS_` 为 **W 隐式合成**（0x0869d6xx 邻域，rep movsl 0x18c
// dwords + word 的整块拷贝，无用户逻辑）⇒ 用户声明的显式定义必产生 T 版逐字段体
// （157/178 DIFF，gap 21）。b28d-37 探针 /tmp/probe_inv.o 已实证**隐式版与 ORIG AE**，
// 旧判词「全仓无 U 引用 ⇒ 无发射点」只说明删声明后不会自然发射，解法即 N84 成功案例
// （QuestScript::C1ERKS_ 849→0）：**删用户声明 + TU 级 force 使用点**。
// CInventory.h 的 `CInventory(const CInventory&)` 声明已由 A24 同批删除（R2 协调）。
// 隐式版语义 = 成员拷贝构造（m_creatureMgr/m_avatarItemMgr 逐个 C1ERKS_ + 其余标量/
// 数组块拷贝），与原显式体逐字段等价。
namespace
{
// force 使用点：仅用于让 4.4 在 -O0 发射隐式 `CInventory::C1ERKS_`（W）与
// `CCreatureMgr::C1ERKS_`/`CAvatarItemMgr::C1ERKS_` 等成员拷贝构造（ORIG 同名 W 符号）。
// 无调用点亦会发射（__attribute__((used)) 保证不被 DCE），不改变任何既有函数体。
__attribute__((used)) void force_emit_CInventory_copy_ops(const CInventory& src, CInventory& dst)
{
    CInventory tmp(src);   // 发射隐式 CInventory::C1ERKS_（W）
    dst = src;             // 发射隐式 CInventory::aSERKS_（W）
    (void)tmp;
}
}  // namespace

// [b28d-round5 N84/P147 **翻正**，取代旧「N84 封顶」判词]
// ORIG `_ZN10CInventoryaSERKS_` = W 隐式合成（0x0869d73c：rep movsl 0x18c dwords + word
// = 0x632B 整块），无用户逻辑；我方旧显式体是 T 版逐字段（116/116 但装载/块拷贝形不同）。
// 与 C1ERKS_ 同批处理：删用户声明（CInventory.h，R2 广播 + Main 批准）+ 删本显式定义，
// 由 TU 级 force 使用点（上）发射隐式 W 版（b28d-37 探针 /tmp/probe_inv.o 实证与 ORIG AE）。
// 语义 = 成员逐个赋值（POD 数组块拷贝 + m_creatureMgr/m_avatarItemMgr 的 aSERKS_），
// 与旧逐字段体完全等价；A32 独立核查全仓无 CInventory 对象级赋值使用点。
// [A32 建议上锁 · b28d-round5] ORIG 总尺寸 0x7b8（CInventory.h 头注释与成员偏移表自洽）；
// 隐式 C1ERKS_/aSERKS_ 的块拷贝长度直接取自 sizeof ⇒ 布局漂移会同时让两件失配（N84）。
static_assert(sizeof(CInventory) == 0x7b8, "CInventory size drift");

#include "CInventory/reset.inc"


#include "CInventory/setCopy.inc"


// ===================== 槽 / 数据访问 =====================

#include "CInventory/CheckValidSlot.inc"


#include "CInventory/GetInvenRef.inc"


#include "CInventory/GetInvenRef_1.inc"


#include "CInventory/GetInvenStart.inc"


#include "CInventory/GetInvenSlot.inc"


#include "CInventory/GetInvenSlotByRef.inc"



#include "CInventory/GetInvenData.inc"


#include "CInventory/SetInvenData.inc"


#include "CInventory/ResetSlot.inc"


#include "CInventory/StartEndPos.inc"


#include "CInventory/getStartEndPos.inc"


#include "CInventory/get_empty_slot.inc"


#include "CInventory/get_inven_slot_no.inc"


#include "CInventory/get_avatar_slot_no.inc"


#include "CInventory/get_item_count.inc"


#include "CInventory/get_item_count_1.inc"


#include "CInventory/InvenEmptycount.inc"


#include "CInventory/QuickEmptyCount.inc"


#include "CInventory/GetRemainCapacity.inc"


#include "CInventory/check_slot_empty.inc"


#include "CInventory/check_empty_count.inc"


#include "CInventory/check_item_exist.inc"


// ===================== 物品类型 / 哈希 =====================

#include "CInventory/GetItemType.inc"


#include "CInventory/GetSlotItemType.inc"


#include "CInventory/getHashCode.inc"


#include "CInventory/getPackedCodeArray1.inc"


#include "CInventory/getSortedKeyArray.inc"


#include "CInventory/makeCheckSum.inc"


// ===================== 货币（第九批 I2 补全） =====================

#include "CInventory/UpdateDailyCoin.inc"


#include "CInventory/UseCoin.inc"


#include "CInventory/UseUpCoin.inc"


#include "CInventory/gain_money.inc"


#include "CInventory/use_money.inc"


#include "CInventory/getValidMoney.inc"


// ===================== 槽 / 数据访问（第九批 I2 补全） =====================

#include "CInventory/GetInvenData_1.inc"


#include "CInventory/GetEmptyCreatureSlot.inc"


#include "CInventory/GetExistCreatureSlot.inc"


#include "CInventory/getAvatarEmptySlot.inc"


#include "CInventory/getEquipWeaponUpgradeSeparate.inc"


#include "CInventory/getExistEquipItem.inc"


#include "CInventory/doesExistEquipItem.inc"


#include "CInventory/MoveItemToEmptySlotOfInventory.inc"


#include "CInventory/IsEquipAvatar.inc"


#include "CInventory/IsExistStackable.inc"


#include "CInventory/IsOverStackableItem.inc"


#include "CInventory/isAnyExpiredAvatarItem.inc"


#include "CInventory/isEquipmentPlaceableAtInventory.inc"


// ===================== 背包插入 / 堆叠（第九批 I2 重点） =====================

#include "CInventory/insertInvenItemIntoQuickSlot.inc"


#include "CInventory/insertInvenItemIntoCoresspondingSpace.inc"


#include "CInventory/stackInvenItemIntoQuickSlot.inc"


#include "CInventory/stackInvenItemIntoCorrespondingSpace.inc"


#include "CInventory/tryInsertInvenItemIntoQuickSlot.inc"


#include "CInventory/tryInsertInvenItemIntoCoresspondingSpace.inc"


#include "CInventory/tryStackInvenItemIntoQuickSlot.inc"


#include "CInventory/tryStackInvenItemIntoCorrespondingSpace.inc"


#include "CInventory/insertItemIntoInventory.inc"


#include "CInventory/tryInsertItemIntoInventory.inc"


#include "CInventory/insertItemIntoEquipment.inc"


#include "CInventory/tryInsertItemIntoEquipment.inc"


#include "CInventory/insertItemIntoCreatureEquipment.inc"


#include "CInventory/insertItemIntoArtifactEquipment.inc"


#include "CInventory/insertItemIntoCreatureStackable.inc"


#include "CInventory/insertItemIntoCreature.inc"


#include "CInventory/tryInsertItemIntoCreature.inc"


// ===================== 日志 / 检查类（第九批 I2 补全） =====================

#include "CInventory/WriteCreatureLog.inc"


#include "CInventory/CheckExistItemInInventoryOrEquipment.inc"


#include "CInventory/check_vilmark_dungeon_item_exist.inc"


#include "CInventory/possible_to_insert.inc"


#include "CInventory/removeDupliacteEquipItem.inc"


#include "CInventory/ShowAllExistItemsForErrorCheck.inc"


#include "CInventory/checkToFix.inc"


#include "CInventory/CheckNFix.inc"


#include "CInventory/ChecknFixWineAndMoru.inc"


// ===================== 专家 Job（第九批 I2 补全） =====================

#include "CInventory/GetExpertJobAddtionalExp.inc"


#include "CInventory/GetExpertJobCompoundMaterialVariation.inc"


#include "CInventory/GetExpertJobCompoundRateVariation.inc"


#include "CInventory/GetExpertJobCompoundResultVariation.inc"


#include "CInventory/GetExpertJobSelfDisjointBigWinRate.inc"


#include "CInventory/GetExpertJobSelfDisjointResultVariation.inc"


// ===================== 物品操作 / 槽处理（第九批 I2 补全） =====================

#include "CInventory/update_item.inc"


// ============================================================================
// GetIntegratedPvPItemAttr（3 参重载，ORIG T 0x084fc651）。MakeItemPacket 直接依赖。
// ORIG 反汇编（84fc651）：G_GameWorld()->IsIntegratedPvPBaseChannel() 且
// G_CDataManager()->GetUpgradeRevisionPvPChannel()==0 时，若 invenType 非 0/非 1
// 且 slot 在 (2, 0x38] 区间 → _getIntegratedPvPItemAttr(item)；否则 → item.GetItemAttr()。
// 返回 unsigned char（ORIG 无 movzbl，值在 %al）。依赖 _getIntegratedPvPItemAttr。
// ============================================================================
unsigned char GetIntegratedPvPItemAttr(const Inven_Item& item, INVEN_TYPE invenType,
                                       int slot)
{
    // ORIG 0x84fc651：外层 && 控制流；内层 switch(invenType)，case 0 空 break
    // （产生 je→nop→GetItemAttr 的 pad 形态），case 1 内做 slot 区间 (2,0x38] 判定。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        switch (invenType)
        {
        case INVEN_TYPE_INVEN:
            break;
        case INVEN_TYPE_EQUIP:
            if (slot > 2 && slot <= 0x38)
            {
                return (unsigned char)_getIntegratedPvPItemAttr(item);
            }
            break;
        }
    }
    return item.GetItemAttr();
}

#include "CInventory/MakeItemPacket.inc"


#include "CInventory/MakeSecuRewarditemPacket.inc"


#include "CInventory/RemoveFPItem.inc"


#include "CInventory/RemoveFPItem_1.inc"


#include "CInventory/RemoveKCItem.inc"

// [N 归因·4.1 pad 形] batch-13 P168a 亲验：同 RemoveFPItem——ORIG 空 continue
// 出口的 nop+jmp 落点 pad 为 GCC 4.1 指令选择，4.4.7 直连 thread 无该形，
// ||/嵌套两式实测 146/138 vs ORIG 142，维持 `||`+continue 形封顶。

#include "CInventory/RemoveKCItem_1.inc"


#include "CInventory/sort.inc"


#include "CInventory/FindItemLock.inc"


#include "CInventory/tryInsertAvatarIntoInventory.inc"


#include "CInventory/insertAvatarIntoInventory.inc"


#include "CInventory/insertAvatarIntoSpecificSlot.inc"


#include "CInventory/use_equip.inc"


#include "CInventory/try_use_item.inc"


#include "CInventory/checkEquipmentState.inc"


#include "CInventory/erase_repeated_item.inc"


// ---- 事件物品批处理（ORIG 0x85058da / 0x85067b2 / 0x8505db4 / 0x8506bb2 / 0x8508fd0 / 0x8509466）----

#include "CInventory/tryDeleteEventItems.inc"


#include "CInventory/tryInsertEventItems.inc"


#include "CInventory/delete_event_items.inc"


#include "CInventory/insert_item_special_slot.inc"


#include "CInventory/MoveClearAvatar.inc"

// [N82 封顶] batch-10 新立项：差集 30=30 归零（parent==0 改 cMyTrace(0x212a,5) 形、
// 0x2c3/0x31a 改 GetInBuffer<SIG_AVATAR_OPTION/COLOR_CHANGE> 就地写域、五处 char buf
// 去 memset——ORIG 未初始化 pad 原样发送，忠实保留）。残余 +13 = 非对齐字段写形差。

#include "CInventory/SendAvatarEvent.inc"


#include "CInventory/ExpiredAvatarItem.inc"


#include "CInventory/CalBlankSlot.inc"


#include "CInventory/CalNeedSlot.inc"

// [N82 封顶] batch-10 新立项：差集 15=15 归零（while+it++ 后置×4、pair 临时&& 赋值、
// 循环头 found 初始化、winPoint !(>=) setge 形、i<=8/i<=0x19 setle 界）。gap 43→7。

#include "CInventory/CheckNeedItemList.inc"


// ===================== 使用 / 装备 / Avatar（第九批 I2 补全） =====================

#include "CInventory/use_item.inc"


#include "CInventory/insertCreatureIntoSpecificSlot.inc"

// [N82 封顶] batch-13：差集归零、207/208 gap=1。残 1 = ORIG 跨调用以 %ecx 存参/装载时刻
// （`mov A(%ebp),%ecx`）vs 4.4.7 spill 直写形 + 尾 nop 豁免差，源层无操作柄。

#include "CInventory/AddAvatarItem.inc"


#include "CInventory/SortInvenSlot.inc"


#include "CInventory/ChangeEquip.inc"


#include "CInventory/IsCreatureItemType.inc"


#include "CInventory/isEquipableItemType.inc"


#include "CInventory/my_compare_unsigned_short.inc"


#include "CInventory/checkStackableLimit.inc"


#include "CInventory/CheckEquipable.inc"


#include "CInventory/getItemChecksum.inc"

#include "CInventory/finishItemChecksum.inc"
{
    if (sum == 0)
    {
        return 0;
    }
    *sum = *sum + (money + coin + a);
    *sum = *sum ^ b;
    return *sum;
}

// ---- 迁移自 GameStubs.cpp 的 CInventory 桩方法（ORIG 符号签名已核对）----
// ORIG _ZN10CInventory15_SaveAvatarItemEiibcRK20stAvatarEmblemInfo_tPKcmRK23stAvatarExpansionInfo_t
// @0x08509e78：m_pParent 为空返回 0；否则经 DB_Q 投递 SIG_AVATAR_INSERT（0x31 头 +
// UID），set(槽位+10/到期/标志/agency/hidden) + setJewelSocket + sig+0x4f 写 expansion
// 前 4 字节，MsgQueueMgr::put(DB_Q)。
namespace
{
// stAvatarExpansionInfo_t（GamePODTypes.h，short m_color[2]）的头 4 字节 int 视角：
// ORIG 0x08509fbf 为单条 movl 读（`mov (%edx),%edx`）⇒ 原码按 4 字节整体读；
// 直写 `*(const int*)&expansion` 时 4.4 走 RHS-first，本 union 具名视角给 LHS-first。
union ExpansionHeadV
{
    short m_color[2];
    int m_head;
};
static_assert(sizeof(ExpansionHeadV) == 4, "ExpansionHeadV == stAvatarExpansionInfo_t");
}  // namespace

#include "CInventory/_SaveAvatarItem.inc"


// ---- SIG_AVATAR_INSERT 非静态成员（ORIG 0x850d44c，this=ins） ----
// ORIG 反汇编逐字段：emblem(+0x00..0x1c) 7 组 dword + 1 word 写入 this+0x2d..+0x4a，
// 与 stAvatarEmblemInfo_t(pack(1), 5×(u16+u32)=0x1e) 完整镜像。
// ORIG 0x850d44c 为 pack(1) 30B 整块 7 dword + 1 word 直拷（跨 stEmblemSocket
// 子字段，无字段语义）；逐子字段写会 10 写 ≠ 8 写而倒退，故用 pack(1) blob
// 视图的具名字段表达（基址仍为具名子对象，消裸偏移）。零扰动（同 disp 同宽）。
namespace
{
#pragma pack(push, 1)
struct EmblemBlobV
{
    int m_d0;               // +0x00
    int m_d1;               // +0x04
    int m_d2;               // +0x08
    int m_d3;               // +0x0c
    int m_d4;               // +0x10
    int m_d5;               // +0x14
    int m_d6;               // +0x18
    unsigned short m_tail;  // +0x1c
};
#pragma pack(pop)
static_assert(sizeof(EmblemBlobV) == 0x1e, "EmblemBlobV == stAvatarEmblemInfo_t size");
}  // namespace

#include "CInventory/setJewelSocket.inc"


#include "CInventory/move_item.inc"

// [N82 封顶] batch-13 专项逐块：差集 80=80 保持归零（4 处 GetAvatarItemMgrR()->
// 改回成员直访 m_avatarItemMgr. + 输出点二次 GetUpgrade(&0x7f)*2 物化）；
// gap 30→1（658/659 长度近等）。定形项：函数头 listCount/count/countIdx 三初始化
// +声明序（ORIG +0x1e）、flag 区 integrated/非 integrated 两组 dead 槽三步折叠
// （未初始化 ampPad/resByte char + 双 `&1u` 掩码复现 and/mov/and 链，ORIG 语义
// 忠实保留）、非 integrated 二次 GetUpgrade 直调、slotNo-0xc unsigned 界、
// (unsigned)<=0x15。残余 ~10 replace 块（ja/jbe 向、shl↔sar 位测折叠、add/lea、
// push/pop esi+ebx）= 跨版本指令选择+寄存器分配，AE 槽号归一口径形态尾差。

#include "CInventory/MakeEquipList.inc"


#include "CInventory/delete_item.inc"


#include "CInventory/insert_event_items.inc"
                                   std::vector<int>& outSlots, eMoneyAddReason moneyReason,
                                   eItemAddReason itemReason, bool bLog,
                                   const char* a, const char* b)
{
    // b28d-solo-CInventory：871/874，N113（ppEi）封顶，语义闭合。
    // b28d-37 [N82/N89 封顶，ORIG 0x8506bb2]：P167 调用点差集 CLEAR（95/95）⇒ 语义闭合；871/874 差为帧/槽与模板内联。
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    int count = 0;
    if (items.size() == 0)
    {
        return count;
    }
    // ORIG 序言块序：pair 默认构造（+0x038）→ 迭代器默认构造（+0x046）→
    // it = items.begin()（+0x06c 赋值，非拷贝初始化）→ item（+0x078）→ totalValue=0。
    std::pair<int, int> cur;
    Iterator it;
    it = items.begin();
    Inven_Item item;
    int totalValue = 0;
    while (it != items.end())
    {
        item.reset();
        // ORIG +0x0bf：临时 pair 物化 + operator=(pair&&)（范式库:358）。
        cur = std::pair<int, int>(*it);
        int itemIdx = cur.first;
        int cnt = cur.second;
        if (itemIdx == 0)
        {
            // ---- 金钱条目 ----
            if (cnt > 499999 && m_pParent != 0)
            {
                // [G8InvenPvP 2026-09-10 · P227 本件实测无净益，已回滚]
                // 两处改全表达式临时链后 ndiff 751→733 但指令 874→878（gap −3→−7，
                // ORIG 871）⇒ 按「指令数不劣化」优先，维持具名形。
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x1dc7, 4);
                trace("Quest reward money(%d) mid(%s) characno(%d) reason(%d)", cnt,
                      NumberToString(m_pParent->get_acc_id(), 0),
                      m_pParent->get_charac_no(-1), (int)moneyReason);
            }
            if (m_pParent != 0 && ((CUserCharacInfo*)m_pParent)->getCurCharacR() != 0)
            {
                // ORIG 0x08506dbd：gain 结果直接覆写 cnt（单槽 -0xe0），
                // 邮件差额判据用 cur.second - cnt。
                cnt = ((CUserCharacInfo*)m_pParent)->getCurCharacInvenW()->
                    gain_money(cnt, moneyReason, true, 0);
            }
            outSlots.push_back(0);
            outSlots.push_back(0);
            outSlots.push_back(cnt);
            ++count;
            it++;   // ORIG：迭代器先推进（+0x283），再判定邮件
            if ((int)moneyReason == 3)
            {
                totalValue += cnt;
            }
            if (cur.second > cnt && bLog)
            {
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, cur.second - cnt, m_pParent->getCurCharacNo(), body,
                    strlen(body), 0, m_pParent->GetServerGroup(), false, false);
            }
            continue;
        }

        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x1e14, 5);
            trace("CInventory::insert_event_items, G_CDataManager()->find_item(%d)",
                  itemIdx);
            it++;
            continue;
        }

        item.m_addInfo = itemIdx;
        if (!pItem->is_stackable())
        {
            if (!pItem->IsAvatarItem())
            {
                if (pItem->IsCreatureItem() && pItem->GetItemType() == 0x16)
                {
                    // ---- 造物蛋 ----
                    if ((int)itemReason != 8)
                    {
                        continue;   // ORIG 怪癖：不推进迭代器
                    }
                    G_CDataManager()->m_itemList->create_item(
                        ENUM_ITEM_CREATE_TYPE_0, item, cnt);
                    if (item.m_field1 == 5)
                    {
                        if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
                        {
                            continue;   // ORIG 怪癖：不推进迭代器
                        }
                        int r = insertItemIntoInventory(item, (eItemAddReason)8, true, false);
                        if (r >= 0)
                        {
                            // [G8InvenPvP 2026-09-10 · 删具名 int 形实测反劣，已回滚]
                            // 曾按 ORIG 0x085076bb/0x085076c8（两值走 %esi/%ebx 无槽）
                            // 改成内联实参；编译后本件 ndiff 751→**926**（大幅反劣）
                            // ⇒ 维持两句具名 int 形（ORIG 值虽在寄存器，但该段的槽
                            // 往返属 4.4 spill 策略，删具名量在 4.4 下并不等价）。
                            int expirationDate = pItem->getExpirationDate();
                            int usablePeriod = pItem->getUsablePeriod();
                            GetCreatureMgrW()->InsertCreatureItem(&item, r, 8,
                                                                  usablePeriod,
                                                                  expirationDate);
                            ++count;
                            outSlots.push_back(r);
                            // ORIG +0xb68：item.m_addInfo 经临时物化走 push_back(int&&)
                            // （其 m_addInfo 为无符号型，本仓声明 int，以 +x 复现转换形态）。
                            outSlots.push_back(+item.m_addInfo);
                            outSlots.push_back(cnt);
                        }
                    }
                }
                else
                {
                    // ---- 普通非堆叠：逐份插入 ----
                    for (int k = 0; k < cnt; ++k)
                    {
                        pItem->make_item(item);
                        int r = insertItemIntoInventory(item, itemReason, true, true);
                        if (r == -1)
                        {
                            if (!bLog)
                            {
                                break;   // ORIG：跳出循环并走共用尾推进
                            }
                            char title[0x15];
                            strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0),
                                    0x14);
                            char body[0x100];
                            strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0),
                                    0xff);
                            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                title, item, 0, m_pParent->getCurCharacNo(), body,
                                strlen(body), 0, m_pParent->GetServerGroup(), false, false);
                            continue;   // 尝试下一份
                        }
                        ++count;
                        outSlots.push_back(r);
                        outSlots.push_back(+item.m_addInfo);
                        outSlots.push_back(item.m_addInfo2);
                        if ((int)itemReason == 8)
                        {
                            int price = 0;
                            G_Store()->GetSellItemPrice(item, pItem,
                                                        (short)item.m_addInfo2, false,
                                                        price);
                            totalValue += price;
                        }
                    }
                }
            }
            else
            {
                // ---- Avatar 物品 ----
                if ((int)itemReason != 8)
                {
                    continue;   // ORIG 怪癖：不推进迭代器
                }
                char ipg[0x18];
                WongWork::CGenUniqueNo::genIPGNo(
                    (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)2,
                    (unsigned int)((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipg);
                // [G8InvenPvP 2026-09-10 · 同上，内联实参形反劣，已回滚]
                int usablePeriod = pItem->getUsablePeriod();
                int r = AddAvatarItem(itemIdx, usablePeriod, false, 0, (char)-1,
                                      ipg, (eAvatarItemAddReason)8, false, 0);
                if (r == -1)
                {
                    if (!bLog)
                    {
                        goto finish_value_statistic;   // ORIG：跳到函数尾统计
                    }
                    WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                        m_pParent, ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), itemIdx,
                        0, '\0', (char)-1, ipg, (char*)0, 0);
                    it++;
                }
                else
                {
                    ++count;
                    outSlots.push_back(r);
                    outSlots.push_back(+item.m_addInfo);
                    int nowTick = GlobalData::s_systemTime_.getCurSec();
                    // [G8InvenPvP G8CInvBig2 复核·无净益] ORIG +0x935→+0x959 调用序
                    // （GetInvenSlot → GetAvatarItemMgrR → GetRemainDate，slot 值经
                    // callee-saved %ebx 跨 call）已由历轮 .o 本处实测复现
                    // （f658/f660/f669/f67c）⇒ 4.4 对链式形本即 slot 先于 mgr；
                    // 「显式限定调用」形对非静态成员非法且零净益 ⇒ 维持原形。
                    outSlots.push_back(GetAvatarItemMgrR()->GetRemainDate(
                        GetInvenSlot(2, r).m_addInfo2, nowTick));
                }
            }
        }
        else
        {
            // ---- 堆叠物品 ----
            item.m_addInfo2 = cnt;
            pItem->make_item(item);
            int r = insertItemIntoInventory(item, itemReason, true, true);
            if (r == -1)
            {
                if (!bLog)
                {
                    goto finish_value_statistic;   // ORIG：跳到函数尾统计
                }
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0, m_pParent->getCurCharacNo(), body, strlen(body), 0,
                    m_pParent->GetServerGroup(), false, false);
                it++;
            }
            else
            {
                ++count;
                outSlots.push_back(r);
                outSlots.push_back(+item.m_addInfo);
                outSlots.push_back(item.m_addInfo2);
                if ((int)itemReason == 8)
                {
                    if (m_pParent != 0)
                    {
                        GetInstanceCubeStatistic()->collectCubeStatistics(
                            pItem, item.m_addInfo2, m_pParent, (CUBE_STATISTIC_FIELD)4);
                    }
                    int price = 0;
                    G_Store()->GetSellItemPrice(item, pItem, (short)item.m_addInfo2, false,
                                                price);
                    totalValue += price;
                }
            }
        }
        // ORIG 共用尾 it++（+0xe7d）：堆叠/头像/造物蛋/非堆叠各成功路径汇流。
        it++;
    }

finish_value_statistic:
    if (((int)itemReason == 8 || (int)moneyReason == 3) && m_pParent != 0)
    {
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xa,
                                                       m_pParent, totalValue);
    }
    return count;
}

#include "CInventory/SendItemLockList.inc"


#include "CInventory/SendItemLockListInven.inc"


#include "CInventory/SendItemLockListAvatar.inc"


#include "CInventory/SendItemLockListCreature.inc"

// [N82 封顶] batch-10 新立项：差集 108=108 归零（删多余 clear()、creature 日期改
// GetCreatureMgrR()、三分支 item 指针缓存全物化为 m_pXxx[slot].field，P183）。
// gap 114→6，残余 6 为槽位 spill 形差。
// [N82 封顶] batch-15 深度块序专项：if 链→switch(invenType)（body 落序
// EQUIP→AVATAR→CREATURE 复现 ORIG cmp $2/$3/$1 决策树 + fall-through 布局，
// difflib 等值率 79.4%）；gap 6→2。定形项：put_header 先行（clear 删除）、
// count=3 槽物化 `put_short(count+3)`、AVATAR/CREATURE `put_short(count)`
// 变量传参、CREATURE tick 先 0 物化。残 2 = push/pop %edi RA 差 + 实参装载
// 次序（4.1 object 先载 vs 4.4 后载，全文件成对 swap 噪声）——版本级行为，勿再迭代。

#include "CInventory/MakeItemList.inc"


// 链接桩：ORIG ChangeEquip 以 4 参自由函数形态调用成员 GetInvenSlot
// （压栈 out,this,1,slot），转发到成员实现。
// [b28d-Main·检查点18] 原自由函数 shim `int GetInvenSlot(Inven_Item*, CInventory*, int, int)`
// 已删：全仓无调用点（成员调用一律走 `->GetInvenSlot(...)`），ORIG 亦无该符号
// （仅成员 `_ZNK10CInventory12GetInvenSlotEii` 0x084fb918）⇒ 纯 only-NEW 发明符号。
