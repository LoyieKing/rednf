// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CInventory/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ============================================================================
// 三个自由辅助函数（ORIG 全局符号，Money 簇自由函数）
//   isGainedGoldFromDungeonReason  @0x850d32a (W)  : reason==4 || reason==0xf
//   write_log_gain_money           @0x84febd9 (T)  : AddGold + 流通统计跳表
//   write_log_use_money            @0x84fed9b (T)  : SubGold + 流通/Value统计跳表
// 语义依据 CInventory.md 逐条对照；amount<0 日志分支忠实 ORIG logFormat 串。
// 注：ORIG 各 case 额外调用 CUser::AddMoneyPlus/AddMoneyMinus(@0x868f266/
//   @0x868f310)，该符号尚未在源实现，此处省略以避免引入新未定义。
// ============================================================================
#include "CInventory/isGainedGoldFromDungeonReason.inc"
}

#include "CInventory/write_log_gain_money.inc"
}

#include "CInventory/write_log_use_money.inc"
}


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
}

#include "CInventory/SetCoin.inc"
}

#include "CInventory/GetPayCoin.inc"
}

#include "CInventory/SetPayCoin.inc"
}

#include "CInventory/GetEventCoin.inc"
}

#include "CInventory/SetEventCoin.inc"
}

#include "CInventory/getTotalCoin.inc"
}

#include "CInventory/IsCoinUsable.inc"
}

#include "CInventory/get_money.inc"
}

#include "CInventory/set_money.inc"
}

#include "CInventory/get_inventory_capacity.inc"
}

#include "CInventory/set_inventory_capacity.inc"
}

#include "CInventory/set_parent.inc"
}

#include "CInventory/SetInventoryMemory.inc"
}

#include "CInventory/GetCreatureMgrW.inc"
}

#include "CInventory/GetCreatureMgrR.inc"
}

#include "CInventory/GetAvatarItemMgrW.inc"
}

#include "CInventory/GetAvatarItemMgrR.inc"
}

#include "CInventory/init.inc"
}

#include "CInventory/GetClearAvatar.inc"
}

#include "CInventory/SetClearAvatar.inc"
}

#include "CInventory/GetClearAvatar_1.inc"
}

#include "CInventory/SetClearAvatar_1.inc"
}

// ORIG 0x8504d0d：比较两个槽位排序键；键 = 槽空(addInfo==0)?0xb:m_field1(ITEM_TYPE)。
#include "CInventory/_CompareSlot.inc"
}

// ORIG C1 0x822d52c W：26 个 m_invenItem 逐元素构造（+0x1c 步进 0x3d）→
// m_pEquipSlot(+0x650)/m_pAvatarSlot(+0x654)/m_pCreatureSlot(+0x6e4) 置 0 →
// AvatarCoin C1(+0x658) → CCreatureMgr C1(+0x6e8) → CAvatarItemMgr C1(+0x734)
// → reset()。成员初始化列表按声明序即复现该序列。
#include "CInventory/CInventory.inc"
}   // 三槽指针(+0x650/+0x654/+0x6e4)声明先于 AvatarCoin/CCreatureMgr 自动构造，故入 init-list(ORIG 序)

// ORIG D1 0x822d600 W：CAvatarItemMgr D1(+0x734) → CCreatureMgr D1(+0x6e8)；
// 其余成员平凡析构，由编译器逆序发射。
#include "CInventory/dtor_CInventory.inc"
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
}

#include "CInventory/setCopy.inc"
}

// ===================== 槽 / 数据访问 =====================

#include "CInventory/CheckValidSlot.inc"
}

#include "CInventory/GetInvenRef.inc"
}

#include "CInventory/GetInvenRef_1.inc"
}

#include "CInventory/GetInvenStart.inc"
}

#include "CInventory/GetInvenSlot.inc"
}

#include "CInventory/GetInvenSlotByRef.inc"
}


#include "CInventory/GetInvenData.inc"
}

#include "CInventory/SetInvenData.inc"
}

#include "CInventory/ResetSlot.inc"
}

#include "CInventory/StartEndPos.inc"
}

#include "CInventory/getStartEndPos.inc"
}

#include "CInventory/get_empty_slot.inc"
}

#include "CInventory/get_inven_slot_no.inc"
}

#include "CInventory/get_avatar_slot_no.inc"
}

#include "CInventory/get_item_count.inc"
}

#include "CInventory/get_item_count_1.inc"
}

#include "CInventory/InvenEmptycount.inc"
}

#include "CInventory/QuickEmptyCount.inc"
}

#include "CInventory/GetRemainCapacity.inc"
}

#include "CInventory/check_slot_empty.inc"
}

// 返回类型 bool（batch-13 Main 终裁，P22：调用点 test %al/xor$1 形均为 bool 产物）；
// 体内 return 1/0 即 return true/false（0/1 语义与全部调用点兼容）。
#include "CInventory/check_empty_count.inc"
}

#include "CInventory/check_item_exist.inc"
}

// ===================== 物品类型 / 哈希 =====================

#include "CInventory/GetItemType.inc"
}

#include "CInventory/GetSlotItemType.inc"
}

#include "CInventory/getHashCode.inc"
}

#include "CInventory/getPackedCodeArray1.inc"
}

#include "CInventory/getSortedKeyArray.inc"
}

// ORIG 0x0850ae7a 返回类型为 int（尾调用 finishItemChecksum 的 eax 流出；
// mangled 不编码返回类型，符号不变）。CUser::getCurCharacInvenCheckSum 直接
// 返回该值（docs/class_func_reports/CUser/getCurCharacInvenCheckSum.md）。
// [N82 封顶] batch-10：差集 7=7 归零（三分支各 1 处 GetUpgrade 独立物化，循环界改 <=）。
// 残余 |gap|=4 = ORIG movzwl 双段零扩展 vs register 变量 spill 形态差（勿再按 ndiff 迭代）。
#include "CInventory/makeCheckSum.inc"
}

// ===================== 货币（第九批 I2 补全） =====================

#include "CInventory/UpdateDailyCoin.inc"
}

#include "CInventory/UseCoin.inc"
}

#include "CInventory/UseUpCoin.inc"
}

// [b28d-PoolMisc2 structdiff 判据] 本件结构残差 **struct=0**、ndiff=8（AE 行的
// `-0xNN(%ebp)`/寄存器/立即数归一后与 ORIG 逐行全等）⇒ 纯 4.4 栈分配差，硬判
// N82附/N95 封顶（工具 /tmp/structdiff.py；同判据件：getHashCode struct=0/ndiff=6）。
#include "CInventory/gain_money.inc"
}

#include "CInventory/use_money.inc"
}

#include "CInventory/getValidMoney.inc"
}

// ===================== 槽 / 数据访问（第九批 I2 补全） =====================

#include "CInventory/GetInvenData_1.inc"
}

#include "CInventory/GetEmptyCreatureSlot.inc"
}

#include "CInventory/GetExistCreatureSlot.inc"
}

#include "CInventory/getAvatarEmptySlot.inc"
}

#include "CInventory/getEquipWeaponUpgradeSeparate.inc"
}

#include "CInventory/getExistEquipItem.inc"
}

#include "CInventory/doesExistEquipItem.inc"
}

#include "CInventory/MoveItemToEmptySlotOfInventory.inc"
}

#include "CInventory/IsEquipAvatar.inc"
}

#include "CInventory/IsExistStackable.inc"
}

// ORIG 0x850a088：entry 即 spill count(0xc,%ebp)->%al,-0x4（volatile 形参强制
// 槽落）。纯常量体会被 4.4 折叠掉 caller 的 call，volatile 挡折叠。
#include "CInventory/IsOverStackableItem.inc"
}

#include "CInventory/isAnyExpiredAvatarItem.inc"
}

#include "CInventory/isEquipmentPlaceableAtInventory.inc"
}

// ===================== 背包插入 / 堆叠（第九批 I2 重点） =====================

#include "CInventory/insertInvenItemIntoQuickSlot.inc"
}

#include "CInventory/insertInvenItemIntoCoresspondingSpace.inc"
}

// [N82 封顶] batch-10：差集 18=18 归零；残余 gap=4 同上（esi/edi 持值形态差）。
#include "CInventory/stackInvenItemIntoQuickSlot.inc"
}

// [N82 封顶] batch-10：调用点差集 20=20 归零（set 处 item.get_add_info 补回）；
// 残余 gap=4 = ORIG push/pop %esi/%edi 跨块持值 vs 本工具链栈槽 spill。
#include "CInventory/stackInvenItemIntoCorrespondingSpace.inc"
}

#include "CInventory/tryInsertInvenItemIntoQuickSlot.inc"
}

#include "CInventory/tryInsertInvenItemIntoCoresspondingSpace.inc"
}

#include "CInventory/tryStackInvenItemIntoQuickSlot.inc"
}

#include "CInventory/tryStackInvenItemIntoCorrespondingSpace.inc"
}

// ORIG T 0x08502d86 (size 0x60b)。
// b28d-PoolMisc2 [P251/P286 **修成**：ndiff 18→7、426→427/428]：`dbLogType != -1`
// 块内 `if (m_field1==1) … else if (m_field1==2) …` 改 **switch (item.m_field1) {
// case 1/break; case 2/break; }** ⇒ 逐条复现 ORIG 0x08502e4c..0x08502e5d 的
// `movzbl 0xd(%ebp); movzbl %al,%eax; cmp $0x1,%eax; je; cmp $0x2,%eax; je; jmp`
// 正向决策链（原 if/else-if 形 4.4 恒产 `cmp $N,%al; jne` 反向链 + 两 case 体
// 错位，共 11 行残差归零）。余 7 行 = ① 0x08502e44 `mov -0x1c(%ebp),%eax;
// cmp $0xffffffff,%eax` vs 4.4 `cmpl $-1,-0x1c(%ebp)`（P75 mem-cmp 折叠）；
// ② 0x08502e67/0x08502ea? 的 `mov -0x1c(%ebp),%ecx/%ebx` 在 receiver 载入
// **之后**取槽（4.4 提前到实参链首，P211 档③装载时刻）⇒ 跨版本 codegen 封顶。
// [b28d-Inventory2 本轮] ② 已修：dbLogType 声明 unsigned int（比较处同步 unsigned）
// ⇒ 三处 `mov -0x1c,%ecx/%ebx` 落回 receiver 之后，gap 1→0 条数 428/427。
// ① 维持封顶：unsigned 局部 + P286 单 case switch + P310 int+cast 三形实测仍产
// `cmpl $-1,-0x1c`（4.4 对局部与常量比较恒 mem 直比，P209/N18b）⇒ 语义完成、
// byte 层余 1 条（p167 CALLSITE CLEAR 21/21）。
#include "CInventory/insertItemIntoInventory.inc"
}

#include "CInventory/tryInsertItemIntoInventory.inc"
}

#include "CInventory/insertItemIntoEquipment.inc"
}

#include "CInventory/tryInsertItemIntoEquipment.inc"
}

#include "CInventory/insertItemIntoCreatureEquipment.inc"
}

#include "CInventory/insertItemIntoArtifactEquipment.inc"
}

#include "CInventory/insertItemIntoCreatureStackable.inc"
}

#include "CInventory/insertItemIntoCreature.inc"
}

#include "CInventory/tryInsertItemIntoCreature.inc"
}

// ===================== 日志 / 检查类（第九批 I2 补全） =====================

#include "CInventory/WriteCreatureLog.inc"
}

// SIGFIX b28c-L2：返回类型 int→bool（ORIG rodata 串 "bool CInventory::CheckExist…"）。
#include "CInventory/CheckExistItemInInventoryOrEquipment.inc"
}

#include "CInventory/check_vilmark_dungeon_item_exist.inc"
}

#include "CInventory/possible_to_insert.inc"
}

#include "CInventory/removeDupliacteEquipItem.inc"
}

#include "CInventory/ShowAllExistItemsForErrorCheck.inc"
}

#include "CInventory/checkToFix.inc"
}

#include "CInventory/CheckNFix.inc"
}

#include "CInventory/ChecknFixWineAndMoru.inc"
}

// ===================== 专家 Job（第九批 I2 补全） =====================

#include "CInventory/GetExpertJobAddtionalExp.inc"
}

#include "CInventory/GetExpertJobCompoundMaterialVariation.inc"
}

#include "CInventory/GetExpertJobCompoundRateVariation.inc"
}

#include "CInventory/GetExpertJobCompoundResultVariation.inc"
}

#include "CInventory/GetExpertJobSelfDisjointBigWinRate.inc"
}

#include "CInventory/GetExpertJobSelfDisjointResultVariation.inc"
}

// ===================== 物品操作 / 槽处理（第九批 I2 补全） =====================

#include "CInventory/update_item.inc"
}

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

// ============================================================================
// CInventory::MakeItemPacket（ORIG 0x084fc6bc，0x1015 字节 / 1073 insn）。
// [N82 封顶标注] batch-10：gap 242→1（1072 vs 1073），ORIG 调用点与 .o R_386_PC32
// 调用多重集 149=149 零差集；残余 1 条 = ORIG 远跳 epilogue 共享多 1 个 `jmp`
// （trace 块布局差异，§6.1/§6.2），另有 add↔lea、shl↔sar、je↔jne 等 N82 形态别名。
// 判据：语义完成、byte 层 N 封顶，勿再按 ndiff 迭代。
// 按 INVEN_TYPE 分 4 组槽位发包：EQUIP(1) 含 0/1/2 三个特殊槽（0=money、
// 1=coin+payCoin+eventCoin、2=GetWinPoint）与普通装备槽；AVATAR(2) 走
// m_pAvatarSlot（含 jewel socket / expansion info）；CREATURE(3) 走
// m_pCreatureSlot（ITEM_TYPE 5 宠时补 GetRemainDate）；INVEN(0) 走
// m_invenItem[slot]（含 PvP 综合通道/封印数分支、avatar 扩展、宠物日期、
// make3rdChroniclePacket）。空槽 / 非期望类型返回 false。
// 依赖：CCreatureMgr::GetRemainDate（头已声明，BLOCKED——producer 提供定义）。
// ============================================================================
#include "CInventory/MakeItemPacket.inc"
}

// ============================================================================
// CInventory::MakeSecuRewarditemPacket（ORIG 0x084fd6d2）。仅 EQUIP(1) 生效：
// 空装备槽返回 0；否则写 put_short(slot)，item_idx 依 m_addInfo2 为 0 时置 -1，
// 再 put_int(m_addInfo2) 并返回 1。
// ============================================================================
#include "CInventory/MakeSecuRewarditemPacket.inc"
}

#include "CInventory/RemoveFPItem.inc"
}

#include "CInventory/RemoveFPItem_1.inc"
}

#include "CInventory/RemoveKCItem.inc"
}
// [N 归因·4.1 pad 形] batch-13 P168a 亲验：同 RemoveFPItem——ORIG 空 continue
// 出口的 nop+jmp 落点 pad 为 GCC 4.1 指令选择，4.4.7 直连 thread 无该形，
// ||/嵌套两式实测 146/138 vs ORIG 142，维持 `||`+continue 形封顶。

#include "CInventory/RemoveKCItem_1.inc"
}

#include "CInventory/sort.inc"
}

#include "CInventory/FindItemLock.inc"
}

#include "CInventory/tryInsertAvatarIntoInventory.inc"
}

#include "CInventory/insertAvatarIntoInventory.inc"
}

#include "CInventory/insertAvatarIntoSpecificSlot.inc"
}

#include "CInventory/use_equip.inc"
}

#include "CInventory/try_use_item.inc"
}

#include "CInventory/checkEquipmentState.inc"
}

#include "CInventory/erase_repeated_item.inc"
}

// ---- 事件物品批处理（ORIG 0x85058da / 0x85067b2 / 0x8505db4 / 0x8506bb2 / 0x8508fd0 / 0x8509466）----

#include "CInventory/tryDeleteEventItems.inc"
}

#include "CInventory/tryInsertEventItems.inc"
}

#include "CInventory/delete_event_items.inc"
}

#include "CInventory/insert_item_special_slot.inc"
}

#include "CInventory/MoveClearAvatar.inc"
}
// [N82 封顶] batch-10 新立项：差集 30=30 归零（parent==0 改 cMyTrace(0x212a,5) 形、
// 0x2c3/0x31a 改 GetInBuffer<SIG_AVATAR_OPTION/COLOR_CHANGE> 就地写域、五处 char buf
// 去 memset——ORIG 未初始化 pad 原样发送，忠实保留）。残余 +13 = 非对齐字段写形差。

#include "CInventory/SendAvatarEvent.inc"
}

#include "CInventory/ExpiredAvatarItem.inc"
}

#include "CInventory/CalBlankSlot.inc"
}

#include "CInventory/CalNeedSlot.inc"
}
// [N82 封顶] batch-10 新立项：差集 15=15 归零（while+it++ 后置×4、pair 临时&& 赋值、
// 循环头 found 初始化、winPoint !(>=) setge 形、i<=8/i<=0x19 setle 界）。gap 43→7。

#include "CInventory/CheckNeedItemList.inc"
}

// ===================== 使用 / 装备 / Avatar（第九批 I2 补全） =====================

#include "CInventory/use_item.inc"
}

#include "CInventory/insertCreatureIntoSpecificSlot.inc"
}
// [N82 封顶] batch-13：差集归零、207/208 gap=1。残 1 = ORIG 跨调用以 %ecx 存参/装载时刻
// （`mov A(%ebp),%ecx`）vs 4.4.7 spill 直写形 + 尾 nop 豁免差，源层无操作柄。

#include "CInventory/AddAvatarItem.inc"
}

// [N82 封顶] batch-13：三处 `start <= i` 翻 `i >= start` 复现 ORIG setge 向（已消除
// replace 块）；残余 gap=5 = ORIG 跨 MoveItemToEmptySlot 调用以 %esi/%ebx 持两下标
// （push/pop esi,ebx prologue）vs 4.4.7 caller-save 直用形，register-asm 强改有
// qsort 段串扰风险，判 N82 不强求。
#include "CInventory/SortInvenSlot.inc"
}

#include "CInventory/ChangeEquip.inc"
}

#include "CInventory/IsCreatureItemType.inc"
}

#include "CInventory/isEquipableItemType.inc"
}

#include "CInventory/my_compare_unsigned_short.inc"
}

#include "CInventory/checkStackableLimit.inc"
}

#include "CInventory/CheckEquipable.inc"
}

#include "CInventory/getItemChecksum.inc"
}
#include "CInventory/finishItemChecksum.inc"
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
}

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
}

// ============================================================================
// CInventory::move_item（ORIG 0x08500688，_ZN10CInventory9move_itemE10INVEN_TYPEiS0_i）。
// [§6.2 闭合 · b20] 帧 604=604 与 ORIG 全等；P167 调用点差集 80/80 归零
// （0 MISSING / 0 EXTRA，CREATURE 分支接收者已按 ORIG 0x08501752 改直接成员
// m_creatureMgr，删 2 处误加 GetCreatureMgrW 调用）；新鲜 .o insns 1279 vs
// ORIG 1282（gap 3）。剩余 byte 层残差属 N82（4.1 不按 lifetime 复用栈槽）
// 与跨版本寄存器物化，判「语义完成、N 封顶」。
// 语义（docs/class_func_reports/CInventory/move_item.md 逐指令对照）：
//   - 双端 CheckValidSlot 失败 → cMyTrace + 0x11；
//   - 源为 0..2 且目的为 CREATURE 时 swap 两端（CREATURE 恒为源）；
//   - EQUIP↔INVEN：isIdentified 成对校验、IsAvatarItem/IsCreatureItem/IsEquipable、
//     IsRandomOption&&m_random.m_random.empty 拒绝、PeriodAfterUnsealing 写 m_fieldb
//     剩余天数、装备时清 m_field0 并 seal_Item 日志、0xd1 服务限制分支；
//   - EQUIP↔EQUIP / AVATAR↔AVATAR 三段交换；AVATAR 分支 MoveClearAvatar/
//     SendAvatarEvent(0x32)；CREATURE 走 CCreatureMgr::SwapCreatureItem。
// [§6.2 巨件核销 · b25] 真修 1 处：EQUIP→AVATAR 段 find_item 后 ORIG 有
// vtbl+0x10(IsAvatarItem)==1 通过 + vtbl+0x14(IsCreatureItem)!=0 拒绝两次虚调用
// （CEquipItem vtable 0x08c96e28 槽 4/5 实证），重建漏写 IsCreatureItem 拒绝分支
// ⇒ 间接虚调用 12→13 与 ORIG 对齐、P167 80/80 保持归零；新鲜 .o 1290 vs 1282，
// 残差归因 N82（槽 -0x20 复用 2 次）+ N80（cMyTrace 变参先物化 edi/esi/ebx）
// + N6（位测 fold）+ 跨版本块排布，判「语义完成、N 封顶」。
// ============================================================================
#include "CInventory/move_item.inc"
}
// [N82 封顶] batch-13 专项逐块：差集 80=80 保持归零（4 处 GetAvatarItemMgrR()->
// 改回成员直访 m_avatarItemMgr. + 输出点二次 GetUpgrade(&0x7f)*2 物化）；
// gap 30→1（658/659 长度近等）。定形项：函数头 listCount/count/countIdx 三初始化
// +声明序（ORIG +0x1e）、flag 区 integrated/非 integrated 两组 dead 槽三步折叠
// （未初始化 ampPad/resByte char + 双 `&1u` 掩码复现 and/mov/and 链，ORIG 语义
// 忠实保留）、非 integrated 二次 GetUpgrade 直调、slotNo-0xc unsigned 界、
// (unsigned)<=0x15。残余 ~10 replace 块（ja/jbe 向、shl↔sar 位测折叠、add/lea、
// push/pop esi+ebx）= 跨版本指令选择+寄存器分配，AE 槽号归一口径形态尾差。

// ============================================================================
// CInventory::MakeEquipList（ORIG 0x084fe2e2，_ZNK10CInventory13MakeEquipListEPvb13ENUM_USERINFO14ENUM_EQUIPSLOT）。
// out 为 InterfacePacketBuf：put_byte 计数占位 → 逐非空槽写（flag=0 精简格式 /
// flag=1 全格式含 amplify、avatar jewel/expansion、creature 剩余天数、
// make3rdChroniclePacket、put_packet）→ 回填计数 → flag=0 时尾部写
// base+0x295（槽 10 m_amp 起始，ORIG 以 UpgradeSeparateInfo::GetUpgradeSeparate
// 单字节读取器跨类型读取）。槽位基址：userInfo==2 或 equipSlot==0 → m_invenItem，
// 否则 CExpandEquipslot::getExpandEquipslotW(equipSlot)（GetCharacExpandData(9)）。
// [§6.2 巨件核销 · b25] 无真修。P167 80/80 保持归零（0 缺 0 多）；新鲜 .o 659 vs
// ORIG 658（gap -1）。残差归因：N6 位测 fold（ORIG `(1<<slot)&0x3ff400` shl 形 vs
// 重建 `(0x3ff400>>slot)&1` sar 形，位测语义等价）、ORIG jl/ja vs 重建 jle/jbe
// 比较方向 fold（slotNo 值域 ≤0x15 无 ≥0x80 歧义，非 P196 真反）、N1 lea/add
// 折叠、xor $1 flag 物化与跨版本块排布，判「语义完成、N 封顶」。
// ============================================================================
#include "CInventory/MakeEquipList.inc"
}

// [N82 封顶] batch-10 续派：差集 39=39 归零；8 处 log 块 parent 判向按 ORIG
// 正向 if(m_pParent!=0){log}else{trace} 翻转后 je/jne 计数逼近（22/8 vs 18/12）。
// 残余 gap=8：ORIG cur-count<0 物化用 add/sub+jle 三 insn 形、cmp/cmpl 与 movl 立即数
// 落槽别名，源层不可控（范式回滚实验见 git 历史），判 N82。
// [b28d-Inventory2 本轮推翻 N82] 625/617 DIFF → 623/623 AE（p167 余 cMyTrace
// C1/C2 同址别名伪差，P93）。三处真因：①外层 if/else 链误译 ⇒ ORIG 0x0850405a
// 是 switch(INVEN_TYPE) 二分决策树（探针实测 4.4.7 同形、叶块序=case 书写序 0,1,2,3，
// P324）；②8 处 cMyTrace 具名局部 ⇒ 全表达式临时链（P221）复现 ORIG 槽阶梯
// -0x88..-0x18；③两处 `cur-count<=0` 早返回 ⇒ 正向 if/else（ORIG `sub;test;jle
// <log 块>`）；另 (INVEN_TYPE)1/0/3 硬编码改回 invenType 形参、删 cur 具名局部
// （P183/P329）。
#include "CInventory/delete_item.inc"
}

// ============================================================================
// CInventory::insert_event_items（ORIG 0x08506bb2，_ZN10CInventory18insert_event_itemsERKSt6vectorISt4pairIiiESaIS2_EERS0_IiSaIiEE15eMoneyAddReason14eItemAddReasonbPKcSD_）。
// items 元素 {itemIdx, count}；itemIdx==0 为金钱条目（>499999 走 cMyTrace 审计，
// gain_money 后 outSlots.push(0,0,gained)，差额邮件兜底）。
// 物品条目按 is_stackable/IsAvatarItem/造物蛋(type 0x16) 三路分发：
//   堆叠 → make_item+insertItemIntoInventory(reason,1,1)；
//   头像 → genIPGNo+AddAvatarItem(itemIdx,usablePeriod,...)+GetRemainDate；
//   造物蛋 → create_item+SetEggItem+insertItemIntoInventory(8,1,0)
//            +InsertCreatureItem；其余非堆叠逐份插入，失败邮件兜底。
// reason==8 时累计 cube 统计与 Store 卖价，函数尾 AddValueStatistic(10)。
// ORIG 怪癖（忠实保留）：头像/造物蛋在 itemReason!=8、SetEggItem 失败时
// 不推进迭代器直接回到循环条件；堆叠/头像插入失败且无邮件时跳到函数尾。
// 依赖：docs/class_func_reports/CInventory/insert_event_items.md。
// [N82 封顶] batch-10 续派：调用点差集 95=95 归零（it++ 后置×5/共用尾、cur=pair 临时
// && 赋值、Iterator 默认构造+赋值、push_back(int&&) 物化 5 处、GetRemainDate 返回值
// 直推、money 段 cnt 覆写单槽形）；残余 |gap|=4 与 jmp/nop/xor 布局为 N82 残差。
// [b28d-tu-CInventory] 本轮复核（P329/方法3 均不适用）：本件无具名中间局部可删
//   （buf 族不存在）；残差主体 = 浅槽全体 4-8B 平移 + 帧 0x2fc vs 0x2dc（我方多
//   8 槽 = 4.4 循环体内 pair/push_back 临时不塌槽，P331 探针1）+ 深槽 bLog/
//   临时组 0x20 平移（4.1 lifetime 复用）⇒ N82 封顶维持。struct 573/ndiff 917。
// ============================================================================
#include "CInventory/insert_event_items.inc"
}

#include "CInventory/SendItemLockList.inc"
}

#include "CInventory/SendItemLockListInven.inc"
}

// ORIG 0x084fafbe / 0x084fafee：分别把 m_pAvatarSlot(0x69 槽)/m_pCreatureSlot(0xf2 槽)
// 的锁清单通过 SendItemLockList 发包（ENUM_ITEMSPACE 1=avatar、7=creature）。
#include "CInventory/SendItemLockListAvatar.inc"
}

#include "CInventory/SendItemLockListCreature.inc"
}
// [N82 封顶] batch-10 新立项：差集 108=108 归零（删多余 clear()、creature 日期改
// GetCreatureMgrR()、三分支 item 指针缓存全物化为 m_pXxx[slot].field，P183）。
// gap 114→6，残余 6 为槽位 spill 形差。
// [N82 封顶] batch-15 深度块序专项：if 链→switch(invenType)（body 落序
// EQUIP→AVATAR→CREATURE 复现 ORIG cmp $2/$3/$1 决策树 + fall-through 布局，
// difflib 等值率 79.4%）；gap 6→2。定形项：put_header 先行（clear 删除）、
// count=3 槽物化 `put_short(count+3)`、AVATAR/CREATURE `put_short(count)`
// 变量传参、CREATURE tick 先 0 物化。残 2 = push/pop %edi RA 差 + 实参装载
// 次序（4.1 object 先载 vs 4.4 后载，全文件成对 swap 噪声）——版本级行为，勿再迭代。

// ============================================================================
// CInventory::MakeItemList（ORIG 0x084fd7b6 T，docs/class_func_reports/
// CInventory/MakeItemList.md 逐指令转写）。
// m_pParent 为空 → cMyTrace 报错返回 0；否则 interface packet 写头 0xd 后按
// INVEN_TYPE 分发：
//   EQUIP(1)：space 0 + 容量 + 计数槽；slot0/1/2 为固定货币/胜负点虚拟条目
//     （addInfo2 分别为 m_money、三币之和、win point），slot3..0x137 遍历
//     m_pEquipSlot 非空槽（IsEnchantRevisionChannel 时 fieldd 置 0、写
//     amplify type/value、make3rdChroniclePacket）；
//   AVATAR(2)：space 1，遍历 m_pAvatarSlot[0..0x68]，含剩余天数/jewel socket/
//     expansion info；
//   CREATURE(3)：space 7，遍历 m_pCreatureSlot[0..0xf1]，ITEM_TYPE 5 宠补
//     GetRemainDate。
// 各分支回填 short 计数后 finalize 返回 1。
// [§6.2 巨件核销 · b25] 无真修。P167 108/108 保持归零（0 缺 0 多）；新鲜 .o
// 726 vs ORIG 728（gap 2）。残差归因：N18a（ORIG `mov %eax,%edx` regmove 中转
// 出参 4.1 形不可造）、N82（全帧槽号平移 0x10 + push edi +4，帧 0x6c vs 0x60）、
// ORIG `lea 0x3(%eax),%edx` vs 重建 `add $0x3,%eax`（N1 族）与 4.1 尾部
// jmp 中继岛布局，判「语义完成、N 封顶」。
// ============================================================================
#include "CInventory/MakeItemList.inc"
}

// 链接桩：ORIG ChangeEquip 以 4 参自由函数形态调用成员 GetInvenSlot
// （压栈 out,this,1,slot），转发到成员实现。
// [b28d-Main·检查点18] 原自由函数 shim `int GetInvenSlot(Inven_Item*, CInventory*, int, int)`
// 已删：全仓无调用点（成员调用一律走 `->GetInvenSlot(...)`），ORIG 亦无该符号
// （仅成员 `_ZNK10CInventory12GetInvenSlotEii` 0x084fb918）⇒ 纯 only-NEW 发明符号。
