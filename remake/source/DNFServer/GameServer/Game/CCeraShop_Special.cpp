// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCeraShop_Special/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r WongWork::CCeraShop 特殊商品三方法 + 依赖（U2 批次）。
// [b28d-solo-CCeraShop_Special 2026-09-09 泳道判词] 清单 14 件逐件核：
//   identical ndiff=0：_processItem / _CheckConditionPutonAvatar / IsSpecialItem
//   （末件 860=860 由 P46/P324 case 块序重排 + P63/P325 共享尾出口 goto 归零）；
//   残差已缩小：_processBoosterSelect 85→68（P183/P157/P323）、
//   _CheckBuyingCondition 357→351（P251/P286）、premium 890→863（lethe 前置）；
//   封 N 族（P321-T5，仅剩装载序/寄存器角色/槽号/lea-vs-add/setcc）：
//   autoUnsealItem N7、inventory_upgrade N7、cargo_abs N4、_onBuySpecialItem N6、
//   ProcessSpecialItem N93、_CheckValidCount / package / _processAdditionalItem /
//   _CheckBuyingCondition / premium N82/N89（§6.2 三件套：p167 全 CLEAR）。
// [b28d-poolcera P326 翻案] ProcessSpecialItem N93 **作废并归零**：end 具名迭代器
// 改右起内联临时形 ⇒ 125/125 ndiff=0（实证并列见函数头判词）。其余 N 件无
// 「双迭代器槽深浅互换」残差形（N82 为帧组成平移、N7 为纯寄存器角色、N4/N6 为
// lea/xor 折叠），复查维持。TU 三数 494/near4/diff11 → 495/near3/diff11。
//   新范式上报 Main：P323（`static_cast<const M&>` 视图 cast 才出 `_ZNK` 重载，
//   内联 `static_cast<const C*>(p)->m.method()` 在 4.4 仍选非 const，探针
//   /tmp/b28d_probe/p2.cpp i vs j）、P324（case 叶块序=书写序，按 ORIG 块地址序重写）、
//   P325（多 return 共享尾出口用 `goto L_true` 尾标签复现）。
// 依据 docs/class_func_reports/WongWork__CCeraShop/{IsSpecialItem,ProcessSpecialItem,
// SaveFeaturedIdx}.md 与 DB_SaveFeaturedGoodsIdx/makeRequest.md、
// STSpecailItem_Param/Result ctor（ORIG W 0x08234ec2/0x08234f36）逐函数转写。

#include "CGameManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CInventory.h"
#include "CStackableItem.h"
#include "CItemList.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "Stream.h"
#include "Inven_Item.h"
#include "CCeraShop.h"
#include "SpecialItemHandler.h"
#include "CHandlePremium.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CPremiumLetheManager.h"
#include "DB_UpdateInventoryCapacity.h"
#include "CEventMgr.h"          // CEventMgr::GetPremiumServiceBonus
#include "SPremiumInfo.h"       // SPremiumInfo 权威布局（g_SPremiumInfo）
#include "GameTypes.h"      // cMyTrace
#include "LogManager.h"     // LogManager::logFormat
#include "CEquipItem.h"     // getAvatarPeriod
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "GameWorld.h"      // G_GameWorld / send_all
#include "STStackableScript.h"  // STStackablePackageData（包数据 ORIG 实证元素类型）
#include "SkillSlot.h"          // SkillSlot 权威布局（getCurCharacSkillR 返回其对象）
// 注：WongWork::CGenUniqueNo 权威声明在 CInventory.h（已含）；勿重复 include CGenUniqueNo.h。
#include "premium_helper_function.h"
#include "DB_UpdateChuseokPackagePurchase.h"  // makeRequest（Chuseok 落库请求）
#include "CItemGenerator.h"  // random_option::CRandomOptionItemHandle::give_option

#include <cstring>
#include <map>
#include <vector>

#include "W8LT_DBTask.h"        // DB_SaveFeaturedGoodsIdx::makeRequest 权威声明



// STSpecailItem_Param/Result 完整定义已提升至 CCeraShop.h（ORIG W ctor
// 0x08234ec2/0x08234f36；布局 +0x00 user +0x04 itemIdx +0x08 value +0x0c items*
// +0x10 ceraParam* +0x14 count +0x18 time(long) +0x1c flag +0x1d/1e/1f char
// +0x20 extra(ctor 置 1)；Result 仅 int m_value）。本 TU 经 CCeraShop.h 获得。

// P254 视图：STSpecailItem_Param 的 m_user 以 CUser* 读取（void* 视图恒多一条
// `mov %eax,%reg` 中转，CUser* 视图直入目标寄存器）。sizeof/偏移/mangled 零影响。
struct STSpecailItem_ParamTypedView
{
    CUser* m_user;                       // +0x00
    int m_itemIdx;                       // +0x04
    int m_value;                         // +0x08（cargo：ORIG 单段直入 %ebx）
};

// P254/P265 视图：m_value 以 enum 类型读取 ⇒ 复现 ORIG 4.1 的两段实参装载
// `mov mem,%eax; mov %eax,%ecx`（giftcard 0x0827de4e 实证）。
struct STSpecailItem_ParamValueEnumView
{
    void* m_user;                        // +0x00
    int m_itemIdx;                       // +0x04
    ENUM_CERA_SPECIALITEM m_value;       // +0x08
};

// P49：寻址助手用宏（static inline 在 -O0 仍产真实 call 符号）。
#define CERA_PARAM_TYPED(p) \
    (reinterpret_cast<const STSpecailItem_ParamTypedView*>(&(p)))
#define CERA_PARAM_VALUE_ENUM(p) \
    (reinterpret_cast<const STSpecailItem_ParamValueEnumView*>(&(p)))

// P82/P95 具名视图：SIG_EXTEND_CHARAC_SLOT 载荷（ORIG 0x0827c0c2 逐字段
// `mov -0xc(%ebp),%edx; mov %eax,(%edx)` / `mov %al,0x4(%edx)` 实证）。
// 用 char* 裸指针访问时 4.4 会把基址缓存进 %ebx 并发 lea；具名成员访问
// 折叠为槽重读 + disp。
struct SlotExtendPayloadView
{
    unsigned int m_accId;        // +0x00（与 get_acc_id() 同型 ⇒ LHS-first）
    unsigned char m_slotEffect;  // +0x04（与 getSlotEffectCount() 同型）
    unsigned char m_slotLimit;   // +0x05（与 getCharacSlotLimit() 同型）
};

// P82 具名视图：SIG_SAVE_CRAZY_LEVELUP_EVENT 载荷（ORIG 0x08327454 逐字段
// `mov -0xc(%ebp),%edx; mov %eax,0x5(%edx)` / `0x9(%edx)` 实证）。
#pragma pack(push, 1)
struct SaveCrazyLevelupPayloadView
{
    char m_tag[5];                // +0x00（"0707\0"）
    unsigned int m_accId;         // +0x05
    int m_characNo;               // +0x09（与 get_charac_no() 同型 ⇒ LHS-first）
};
#pragma pack(pop)
// ============================================================================
// [round5 TU 拆分 2026-09-10] **部件包含式聚合器**：全部实现已按「一函数一部件」
// 迁至同目录 Game/CCeraShop_Special/*.inc（部件不参与单独编译、内部**无 #include**，
// 头/宏/视图全部由本前言提供）。此处按**原定义序**逐个 #include ⇒ TU 名、旗标、
// 头集、后端与拆分前逐项一致 ⇒ codegen 由构造保证不变（GI 键仍 = 首个强符号
// SpecialItemHandler::C2；__ioinit / __static_initialization_and_destruction_0ii
// 仍每 TU 各一份）。顺序表与分组依据见 local://split_CCeraShop_Special.json。
// 未使用 __FILE__/__LINE__/__DATE__/__TIME__（grep 实证；仅 __PRETTY_FUNCTION__，
// 其值由函数签名决定，与文件名无关）。
// ============================================================================

// ============================================================================
// SpecialItemHandler —— ORIG 全局类（mangled _ZN18SpecialItemHandler…，非 WongWork 前缀）。
// 权威声明见 SpecialItemHandler.h；本 TU 为唯一实现点。
// ctor 0x0827bcec / execute 0x0827be0a / respond_* 家族地址见
// docs/class_func_reports/SpecialItemHandler/README.md。
// ============================================================================

#include "CCeraShop_Special/SpecialItemHandler.inc"


#include "CCeraShop_Special/execute.inc"


// ============================================================================
// respond_special_item_* 家族
// ============================================================================

#include "CCeraShop_Special/respond_special_item_cargo.inc"


#include "CCeraShop_Special/respond_special_item_cargo_abs.inc"


#include "CCeraShop_Special/respond_special_item_gift_ticket.inc"


#include "CCeraShop_Special/respond_special_item_skill_confirm.inc"


#include "CCeraShop_Special/respond_special_item_skill_tree_style.inc"


#include "CCeraShop_Special/respond_special_item_booster_selection.inc"


#include "CCeraShop_Special/respond_special_item_cerapoint_giftcard.inc"


#include "CCeraShop_Special/respond_special_item_charac_slot_extend.inc"


#include "CCeraShop_Special/respond_special_item_inventory_upgrade.inc"


#include "CCeraShop_Special/respond_special_item_booster.inc"


#include "CCeraShop_Special/respond_special_item_package.inc"


#include "CCeraShop_Special/respond_special_item_premium.inc"


#include "CCeraShop_Special/cal_premium_term.inc"


#include "CCeraShop_Special/cal_end_time.inc"


#include "CCeraShop_Special/get_target_server.inc"


#include "CCeraShop_Special/send_db.inc"


#include "CCeraShop_Special/is_notify.inc"


#include "CCeraShop_Special/write_log.inc"
{
    switch (type)
    {
    case 0x53:
    case 0x57:
        user->ReCalcChattingEmoticon();
        user->SendChattingEmoticon();
        break;
    case 8:
    {
        PacketGuard packet;
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);
        break;
    }
    }
}

namespace WongWork
{

#include "CCeraShop_Special/checkBuyingCondition.inc"


#include "CCeraShop_Special/_CheckConditionPutonAvatar.inc"


#include "CCeraShop_Special/_CheckBuyingCondition.inc"


// ============================================================================
// IsSpecialItem —— ORIG 0x08324a1e (T)
// 固定商品表：itemId → (type, value, flag)；表外回退 CDataManager 物品类型判定。
// §6.2 巨件核销：P167 3/3 [0缺0多]；gap -25→-63；残差归因 N46/N86 + N7
// （稀疏 switch 决策树 4.1/4.4 划分算法结构性差：ORIG 首层 0x334 vs 4.4 首层
//   0x2e8，cmp 值序列两侧树形不同、条数 85/83；虚调用 callee 寄存器
//   call *%edx↔*%eax 与载荷序差。case 集与 (type,value,flag) 映射同 Ghidra C
//   逐一核对一致。真修：默认路径 GetItemType() 由缓存 1 次改回 ORIG 的 5 次
//   独立虚调用（0x08257b..0x08325 +0xd5b/+0xd83/+0xdb8/+0xdcf/+0xe0c 实证，
//   P0：多 call ⇒ 源码无缓存）。
// ============================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x08324a1e orig/ours=860/923 ndiff=801。
// 巨件 switch 决策树 + 帧/槽号平移（N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL 860/860 ndiff=0 [原 DIFF 936]：
// ①真语义 bug 修复 case 2660545（0x2898C1 → 2/0x54，ORIG 0x083255cf 块）；②P46/P324
// case 叶块物理顺序=书写顺序 ⇒ 按 ORIG 块地址序 0x08324eaf..0x0832570e 重写 case 表
// （决策树与 case 序无关，树形逐条不变）；③P63/P325 ORIG 全 case 共用尾出口
// +0xe2f `mov $0x1,%eax` ⇒ 统一 `goto L_true`（default 路径包块以合法跨越）。
// P167 CLEAR 3/3。
#define CERA_SPECIAL_CASE(idx, t, v) \
    case idx: outType = t; outValue = v; goto L_true
#define CERA_SPECIAL_CASE_F(idx, t, v, f) \
    case idx: outType = t; outValue = v; flag = f; goto L_true
#include "CCeraShop_Special/IsSpecialItem.inc"


#undef CERA_SPECIAL_CASE
#undef CERA_SPECIAL_CASE_F

#include "CCeraShop_Special/ProcessSpecialItem.inc"


#include "CCeraShop_Special/_CheckBuyingCondition_1.inc"


#include "CCeraShop_Special/_CheckValidCount.inc"


#include "CCeraShop_Special/ProcessError.inc"


#include "CCeraShop_Special/SaveFeaturedIdx.inc"


#include "CCeraShop_Special/setOriginalItem.inc"


#include "CCeraShop_Special/getOriginalItem.inc"


// ============================================================================
// Is2010ChuseokPacket —— ORIG 0x083280dc (W, size 0x23)
// 2010 秋夕（Chuseok）包商品判定：itemIdx ∈ (0x2896d3, 0x2896ef]。
// 判据㉓ 符号绑定：ORIG nm 为 W（弱定义，ORIG 头内 inline 族），我方 T ⇒
// 本 TU 定义加 weak（不改头；mangled/体尺寸 0x23 不变）。
// ============================================================================
__attribute__((weak)) bool CCeraShop::Is2010ChuseokPacket(unsigned int itemIdx)
{
    return itemIdx > 0x2896d3 && itemIdx <= 0x2896ef;
}

#include "CCeraShop_Special/_processCoin.inc"


#include "CCeraShop_Special/autoUnsealItem.inc"


#include "CCeraShop_Special/_processItem.inc"


#include "CCeraShop_Special/_processCreature.inc"


#include "CCeraShop_Special/_sendCargoUpgradePacket.inc"


#include "CCeraShop_Special/_sendCharacSlotExtendPacket.inc"


#include "CCeraShop_Special/_sendInventoryUpgradePacket.inc"


#include "CCeraShop_Special/_sendPremiumPacket.inc"


#include "CCeraShop_Special/_ClosePremium.inc"


#include "CCeraShop_Special/_onBuySpecialItem.inc"


#include "CCeraShop_Special/_processBoosterSelect.inc"


#include "CCeraShop_Special/_processAdditionalItem.inc"

}  // namespace WongWork

#include "CCeraShop_Special/makeRequest.inc"

