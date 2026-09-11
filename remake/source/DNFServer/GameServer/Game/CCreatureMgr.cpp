// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCreatureMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <string.h>

#include <map>
#include <algorithm>   // std::swap<Inven_Item>（ORIG _ZSt4swapI10Inven_ItemEvRT_S2_）

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CEquipItem.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CCreatureScriptMgr.h"  // user_creature::GetInstanceCreatureScriptMgr / FindCreatureScript
#include "CPacketRespondent.h"
#include "CCreatureScript.h"   // user_creature::CCreatureScript 访问器（isMaxLevel/vaildItemExp）
#include "TimerQueue.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "GameTypes.h"
// itemGloballyUniqueIdentifier_t（Inven_Item +0x15 打包全局唯一标识区，
// Inven_Item_Compatible::setCopy/makeInvenItem 的具名访问依据）。
#include "ItemGloballyUniqueIdentifierGenerator.h"

// SetEggItem 依赖的随机数（定义于 DNFFunctionLib.cpp，全局 T _Z11get_rand_inti）。
int get_rand_int(int range);

namespace user_creature
{

// ============================================================================
// CCreature（ORIG 0x833705e ctor / 0x83370b0 Reset）
// ============================================================================

#include "CCreatureMgr/SetStomachValue.inc"

#include "CCreatureMgr/GetStomachValue.inc"

#include "CCreatureMgr/IncrementValue.inc"

#include "CCreatureMgr/Reset.inc"

#include "CCreatureMgr/CCreature.inc"

// ~CCreature 定义迁至 CGameManager.cpp（CNode 特化之前）：P210 callee 定义顺序
// 定律——DynamicPool<CCreature>::CNode::~CNode 需先可见 dtor 定义才能继承
// ORIG 的 sub $0x4 小帧（仅声明时 4.4 保守撑至 0x18；探针 /tmp/bm3/）。

#include "CCreatureMgr/GetState.inc"

#include "CCreatureMgr/IsDieCreature.inc"

#include "CCreatureMgr/SetState.inc"

#include "CCreatureMgr/isMaxLevel.inc"

#include "CCreatureMgr/vaildItemExp.inc"
#include "CCreatureMgr/GainExp.inc"

// ============================================================================
// GainExp 依赖链（CreatureExperience 批次；逐函数按 reports/user_creature__CCreature
// 汇编转写。字段全部具名访问，无裸偏移；exp 打包语义：低字节=m_field4d（A/B 型
// 平衡值），高 16 位=成长进度计数。）
// ============================================================================

#include "CCreatureMgr/GetCreatureId.inc"

#include "CCreatureMgr/SetItemId.inc"

#include "CCreatureMgr/IsAvailableArtifact.inc"

#include "CCreatureMgr/GetExpLevel.inc"

// ORIG _ZN13user_creature9CCreature9GetAddExpEiP5CUser T 0x8337724（GetAddExp.md）：
// 槽位可用时取当前角色背包内 CCreatureMgr（ORIG invenW+0x6e8，镜像具名成员）
// 的 GetArtifact(slot)；神器存在且（最低等级为 0 或 <=m_level）返回其经验量，
// 否则 0.0f。
namespace
{
    // ORIG 0x833775a `lea 0x6e8(%eax),%edx`：GetCreatureMgrW 为 4.1 头内联的
    // 成员地址计算（无 call、无 extra 槽）。本 TU 以 pad 镜像视图复现单级 lea。
    struct CInventoryCreatureMgrView
    {
        char m_prefix[0x6e8];
        user_creature::CCreatureMgr m_creatureMgr;  // CInventory+0x6e8
    };
}
#include "CCreatureMgr/GetAddExp.inc"

#include "CCreatureMgr/CalculateExp.inc"

#include "CCreatureMgr/Evolute.inc"

#include "CCreatureMgr/SelectEvolute.inc"

#include "CCreatureMgr/GainExpCreature.inc"

#include "CCreatureMgr/GainItemExpCreature.inc"

// ============================================================================
// ORIG W 访问器（0x833f04c..0x833f0e6；本 TU 提供强定义）
// ============================================================================

#include "CCreatureMgr/IsSave.inc"

#include "CCreatureMgr/GetStomach.inc"

#include "CCreatureMgr/GetExp.inc"

#include "CCreatureMgr/IsNoCharge.inc"

#include "CCreatureMgr/SetNoCharge.inc"

#include "CCreatureMgr/GetCreatureId_1.inc"

#include "CCreatureMgr/GetOverSkillTime.inc"

#include "CCreatureMgr/UseFeed.inc"

#include "CCreatureMgr/dtor_CCreatureMgr.inc"

// ORIG operator= @ 0x869d614【objdump 逐条转写】：
// map aSE → +0x18/+0x1c 指针 → +0x20/+0x24/+0x28 CArtifact 三条平铺 store →
// +0x2c → map<int,int> aSE(+0x30) → movzbl +0x48 → 返回 this。
// [b28d-23 N·4.1-CSE] 三 artifact store 共享 %eax/%edx 基址（ORIG 跨语句 CSE）；P206 8B 视图形/register 钉基址均反劣 ⇒ N。ORIG 0x0869d614
// [b28d-solo-CCreatureMgr AE] P247 ArtifactTailView 12B 结构赋值复现共享基址+%ecx 缓存块拷贝；ORIG 0x869d614
CCreatureMgr& CCreatureMgr::operator=(const CCreatureMgr& src)
{
    m_creatureItems = src.m_creatureItems;
    m_equippedCreature = src.m_equippedCreature;
    m_user = src.m_user;
    // [b28d-solo-CCreatureMgr P247/P206] ORIG 0x869d644 三条 artifact store 共享
    // %eax(dest)/%edx(src)、value 走 %ecx（12B 块拷贝形）；逐元素赋值 4.4 恒
    // 每条重载双基址。改用 12B 视图结构整体赋值复现块拷贝（布局/mangled 不变）。
    struct ArtifactTriple { int m_w0, m_w1, m_w2; };
    struct ArtifactTailView { char m_pre[0x20]; ArtifactTriple m_art; };
    reinterpret_cast<ArtifactTailView*>(this)->m_art =
        reinterpret_cast<const ArtifactTailView*>(&src)->m_art;
    m_tempExpireDelta = src.m_tempExpireDelta;
    m_field30 = src.m_field30;
    m_isMaxLevelGrowCreature = src.m_isMaxLevelGrowCreature;
    return *this;
}

#include "CCreatureMgr/Reset_1.inc"

#include "CCreatureMgr/SetUser.inc"

#include "CCreatureMgr/UseItem.inc"

#include "CCreatureMgr/UseFeed_1.inc"

#include "CCreatureMgr/GainExp_1.inc"

#include "CCreatureMgr/IsEquippedCreature.inc"

#include "CCreatureMgr/IsGrowCreature_Equipped_Creature.inc"

#include "CCreatureMgr/IsGrowCreature.inc"

#include "CCreatureMgr/setTempExpireTime.inc"
#include "CCreatureMgr/InsertCreatureItem.inc"

#include "CCreatureMgr/SendCreatureItemList.inc"

}  // namespace user_creature

namespace user_creature
{

#include "CCreatureMgr/GetCreatureItemId.inc"

#include "CCreatureMgr/GetCreatureItem.inc"

#include "CCreatureMgr/GetCreatureName.inc"

#include "CCreatureMgr/GetName.inc"

#include "CCreatureMgr/IsDieCreature_1.inc"

#include "CCreatureMgr/GetEquipedCreatureLevel.inc"

#include "CCreatureMgr/GetLevel.inc"


#include "CCreatureMgr/GetArtifact.inc"


// ============================================================================
// 本批补齐（U3）：IsCreatureEquipmentScope / RecvMail / RegistTempPeriod
// ============================================================================

#include "CCreatureMgr/IsCreatureEquipmentScope.inc"

#include "CCreatureMgr/RegistTempPeriod.inc"

// ============================================================================
// GetRemainDate —— ORIG _ZNK13user_creature13CCreatureItem13GetRemainDateEi
//   W 0x0833f016（CCreatureItem）与
//   _ZNK13user_creature12CCreatureMgr13GetRemainDateEii T 0x0833d78e（CCreatureMgr）。
// 支持 CInventory::MakeItemPacket CREATURE/INVEN 分支
//   buf.put_int(m_creatureMgr.GetRemainDate(item->m_addInfo2, nowTick))。
// ============================================================================

#include "CCreatureMgr/GetRemainDate.inc"

#include "CCreatureMgr/GetRemainDate_1.inc"

#include "CCreatureMgr/RecvMail.inc"

#include "CCreatureMgr/SendMail.inc"

#include "CCreatureMgr/DeleteCreatureItem.inc"

#include "CCreatureMgr/UnregisterCreatureItem.inc"

// ============================================================================
// user_creature::CDBMsgSender（ORIG 域内类，本 TU 提供最小真实实现）
// ============================================================================

#include "CCreatureMgr/CDBMsgSender.inc"

// Init 定义已迁移至 CCreature.cpp（ORIG W 0x833f7a8 逐条转写；本 TU 仅保留
// C1 委托调用，避免两 TU 强符号冲突；全局 ::ENUM_INTERNALPACKET 见 CacheCharacterType.h）。

#include "CCreatureMgr/dtor_CDBMsgSender.inc"

#include "CCreatureMgr/Send.inc"

#include "CCreatureMgr/RecvMail_1.inc"

#include "CCreatureMgr/SendMail_1.inc"

#include "CCreatureMgr/UseFeedCreature.inc"

#include "CCreatureMgr/UseKeepRight.inc"

#include "CCreatureMgr/EvoluteCreature.inc"

#include "CCreatureMgr/InsertCreatureItem_1.inc"

#include "CCreatureMgr/SetEggItem.inc"


// ============================================================================
// 进化资格校验链（RestoreUserQuestStubs：Dispatcher_VerifyCreatureQuest 依赖）
// ============================================================================

#include "CCreatureMgr/IsAbleEvolute.inc"

#include "CCreatureMgr/IsAbleSelectEvolute.inc"

#include "CCreatureMgr/VerifyEquipedCreatureQuest.inc"

// ============================================================================
// SwapCreatureItem 调用链（Prod_DB_Misc 批次；逐函数对照
// docs/class_func_reports/user_creature__CCreatureMgr/ 与 user_creature__CCreature/
// 的 ## 汇编 + ## 反编译 C 转写，符号与 ORIG mangled 一致）
// ============================================================================

#include "CCreatureMgr/SetSlotNo.inc"

#include "CCreatureMgr/GetCreatureItemType.inc"

#include "CCreatureMgr/checkTime.inc"

#include "CCreatureMgr/Equip.inc"

#include "CCreatureMgr/Dismantle.inc"

#include "CCreatureMgr/SwapCreatureItem.inc"

#include "CCreatureMgr/IsArtifactEquipSlot.inc"

#include "CCreatureMgr/EquipArtifact.inc"

#include "CCreatureMgr/DismantleArtifact.inc"

#include "CCreatureMgr/Equip_1.inc"

#include "CCreatureMgr/Dismantle_1.inc"

#include "CCreatureMgr/EquipCreatureItem.inc"

// ============================================================================
// CheckItem / SetKeepRightItem（Cerashop creature 商品购买预检链）
// ============================================================================

#include "CCreatureMgr/SetKeepRightItem.inc"

#include "CCreatureMgr/CheckItem.inc"

#include "CCreatureMgr/GetExtraGold.inc"

#include "CCreatureMgr/GetOverSkillExtraGold.inc"

#include "CCreatureMgr/SwapCreature.inc"

#include "CCreatureMgr/SwapArtifact.inc"

#include "CCreatureMgr/SwapCreatureItem_1.inc"

#include "CCreatureMgr/ResponseCreature.inc"

#include "CCreatureMgr/RegisterCreatureItem.inc"

#include "CCreatureMgr/RenameCreatureItem.inc"

#include "CCreatureMgr/HatchCreature.inc"
#include "CCreatureMgr/RenameCreature.inc"

}  // namespace user_creature

// ============================================================================
// Inven_Item_Compatible（全局域，ORIG 0x833f38e/0x833f418；U3Creature 批次实现）
// ============================================================================

#include "CCreatureMgr/setCopy.inc"

#include "CCreatureMgr/makeInvenItem.inc"

// ============================================================================
// CCreatureItemFactory（ORIG ctor 0x0833f67a / dtor 0x0833f6d0 / guard 单例）
//   两 DynamicPool：+0x00 CEgg 池、+0x5c CCreature 池，各 0x3e8（1000）基数。
//   AcquireCreatureItem(type)：type==1 → CCreature 池，type==0 → CEgg 池；
//   Acquire 后经 vtable+0 调 C1 构造。失败 cMyTrace 报错并返回 0。
//   FreeCreatureItem(item)：按池归还。
// ============================================================================
// [AnonLane3 R14 · P154] 删除发明死常量 (anonymous namespace)::kCreativeFactoryTrace
//   （.data 4B 指针 + 所指数串；本 TU 零使用点——AcquireCreatureItem.inc 已按 P139 直用
//   __PRETTY_FUNCTION__；ORIG 全库无该 mangled 名，nm 零命中）⇒ 仅NEW 噪音，删定义。

#include "CCreatureMgr/CCreatureItemFactory.inc"

#include "CCreatureMgr/dtor_CCreatureItemFactory.inc"

#include "CCreatureMgr/AcquireCreatureItem.inc"

#include "CCreatureMgr/FreeCreatureItem.inc"

#include "CCreatureMgr/GetInstanceCreatureItemFactory.inc"

#include "CCreatureMgr/Hatch.inc"

#include "CCreatureMgr/SetStomach.inc"

#include "CCreatureMgr/Set.inc"

#include "CCreatureMgr/Hatch_1.inc"

#include "CCreatureMgr/GetSize.inc"

#include "CCreatureMgr/GetRemainSize.inc"
