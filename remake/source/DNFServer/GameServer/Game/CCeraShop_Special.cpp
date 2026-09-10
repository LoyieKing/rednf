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

// ---- SpecialItemHandler::SpecialItemHandler —— ORIG 0x0827bcec ----
// 对象布局：+0x00 CCeraShop* m_owner；+0x04.. 14 组 {respond fn, offset} 派发表。
// ctor 先清零 14 组，再按 type 填 dispatch 表（respond fn 地址，offset 全 0）。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] PMF 赋值成对 store 复现 ORIG fn+adj 写
#include "CCeraShop_Special/SpecialItemHandler.inc"
{
    // b28d-solo：Entry.fn 改真实 PMF（见 SpecialItemHandler.h 判据）。PMF 赋值
    // 在 4.4 发 ptr+adj 双 store，与 ORIG 每项 `movl $fn,disp; movl $0,disp+4`
    // 成对写同形。
    m_owner = owner;
    for (int i = 0; i < 0xe; ++i)
        m_entries[i].fn = 0;
    // ORIG 0x0827bcec：按登记表序写 PMF（ptr + adj=0 成对 store）。
    m_entries[1].fn = &SpecialItemHandler::respond_special_item_cargo;
    m_entries[7].fn = &SpecialItemHandler::respond_special_item_cargo_abs;
    m_entries[11].fn = &SpecialItemHandler::respond_special_item_inventory_upgrade;
    m_entries[8].fn = &SpecialItemHandler::respond_special_item_charac_slot_extend;
    m_entries[5].fn = &SpecialItemHandler::respond_special_item_package;
    m_entries[6].fn = &SpecialItemHandler::respond_special_item_booster;
    m_entries[12].fn = &SpecialItemHandler::respond_special_item_booster_selection;
    m_entries[9].fn = &SpecialItemHandler::respond_special_item_skill_confirm;
    m_entries[10].fn = &SpecialItemHandler::respond_special_item_skill_tree_style;
    m_entries[2].fn = &SpecialItemHandler::respond_special_item_premium;
    m_entries[13].fn = &SpecialItemHandler::respond_special_item_cerapoint_giftcard;
}

// ---- SpecialItemHandler::execute —— ORIG 0x0827be0a ----
// 表驱动分发：type<0xe 时取 m_entries[type] 的 respond PMF 调用。
// ORIG 用 4.1 PMF 调用序列（ptr bit0 虚判定 + adj this 调整）；本对象所有条目
// 均为非虚成员函数（adj=0、ptr bit0=0），itoc 直接路径。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] 真实 PMF（SpecialItemHandler.h Entry::fn）复现 4.1 itoc 调用序列
#include "CCeraShop_Special/execute.inc"
                                 STSpecailItem_Result& result)
{
    if (type > 0xd)
        return false;
    (this->*m_entries[type].fn)(type, param, result);
    return true;
}

// ============================================================================
// respond_special_item_* 家族
// ============================================================================

// ---- respond_special_item_cargo —— ORIG 0x0827be8a ----
// 仓库扩容：getCurCharacCargoW 上调 UpgradeCargo(value)；成功发 cargo 升级包，
// 失败 result.m_value = 0x17。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P292 UpgradeCargo bool 化 + CUser* 视图 + P59 负极性守卫
#include "CCeraShop_Special/respond_special_item_cargo.inc"
                                                    const STSpecailItem_Param& param,
                                                    STSpecailItem_Result& result)
{
    // b28d-D42 P0/P52：ORIG 0x0827be8a 无 user/value 具名槽——value 先入 %ebx
    // 跨 getCurCharacCargoW call 保活，user 在成功臂逐点重读 param.m_user
    // （两次 `mov (%eax),%edx`），故写成单条链式表达式。
    // b28d-D42：ORIG 0x0827be8a 为「失败块直落 + jmp 尾、成功块在尾」，
    // 判定 `xor $1,%eax; test %al,%al; je <成功>`（4.1 对 x==1 的 xor 展开）。
    // 4.4 的 if/else 恒 then-first（je 落 else）⇒ 条件取反成 `!= 1` 使失败块直落；
    // 残余 xor vs cmp 为 4.4 折叠差异（N18a 同族，见判词）。
    if (!CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoW()->UpgradeCargo(
            CERA_PARAM_TYPED(param)->m_value))
        result.m_value = 0x17;
    else
        m_owner->_sendCargoUpgradePacket(CERA_PARAM_TYPED(param)->m_user, type);
}

// ---- respond_special_item_cargo_abs —— ORIG 0x0827bee6 ----
// 绝对容量仓库升级：目标容量 = 当前 GetCapacity()+value；isUpgradable 通过则
// UpgradeCargo 后发包，否则 result.m_value = 0x12。
// b28d-solo-CCeraShop_Special verdict=DIFF [N4] 55/57；余 2 处 lea(%eax,%edx,1) vs add+mov。
// P167 CLEAR 9/9；P248 三档类型旋钮已试（字段/callee 返回类型/同异型）不可择 ⇒ N4/N18a 封顶。
#include "CCeraShop_Special/respond_special_item_cargo_abs.inc"
                                                        const STSpecailItem_Param& param,
                                                        STSpecailItem_Result& result)
{
    // b28d-D42 verdict=DIFF [N18a+N82] ORIG 0x0827bee6 orig/ours=55/57 ndiff=39。
    // P0/P52 去具名槽后余 ORIG 两段 `mov mem,%eax; mov %eax,%ebx` 中转被 4.4
    // 折单段（N18a）+ 帧差；语义等价封顶。
    // b28d-D42 P0/P52：ORIG 0x0827bee6 无 user/value 具名槽——param.m_value
    // 从载荷直读入 %edx 与 capacity 相加后存 %ebx 跨 call 保活；user 逐点重读
    // `mov 0x10(%ebp),%eax; mov (%eax),%eax`（P48 无 CSE）。
    if (!CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoR()->isUpgradable(
            ({ CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoR()->GetCapacity() +
               CERA_PARAM_TYPED(param)->m_value; })))
    {
        result.m_value = 0x12;
    }
    else
    {
        CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoW()->UpgradeCargo(
            ({ CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoR()->GetCapacity() +
               CERA_PARAM_TYPED(param)->m_value; }));
        m_owner->_sendCargoUpgradePacket(CERA_PARAM_TYPED(param)->m_user, type);
    }
}

// ORIG 仅读取 param 的 m_user/m_value 到局部后返回（O0 保留死读取）。
#include "CCeraShop_Special/respond_special_item_gift_ticket.inc"
                                                          const STSpecailItem_Param& param,
                                                          STSpecailItem_Result&)
{
    void* user = param.m_user;
    int value = param.m_value;
}

// ---- respond_special_item_skill_confirm —— ORIG 0x0827cdd0 ----
// 日志后调用 CPremiumLetheManager::ConfirmSkillReq(user)。
#include "CCeraShop_Special/respond_special_item_skill_confirm.inc"
                                                            const STSpecailItem_Param& param,
                                                            STSpecailItem_Result&)
{
    // b28d-D42 P28/P0：ORIG 0x0827cdd0 先取 mgr（-0x1c 槽）；user 无具名槽
    // （每处 `mov 0x10(%ebp),%eax; mov (%eax),%eax` 逐点重读 param.m_user）。
    CPremiumLetheManager* mgr = G_CGameManager()->GetPremiumLetheManager();
    // b28d-D42 P227/P87：ORIG 0x0827cdd0 在 cMyTrace ctor 前右起求 varargs
    // 并保活 edi/esi/ebx（style→edi、characNo→esi、charName→ebx）⇒ 全表达式
    // 临时链形（具名三局部会退化为栈槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x1e1, 0)(
        "ONE_DAY_LETHE : BUY_SKILL_CONFIRM_TICKET, char(%s), char_no(%d), style(%d)",
        static_cast<CUser*>(param.m_user)->getCurCharacName(),
        static_cast<CUser*>(param.m_user)->getCurCharacNo(),
        static_cast<CUser*>(param.m_user)->GetCurCharacSkillTreeIndex());
    // b28d-D42 P223 变体：ORIG 0x0827cdd0 尾部为 `jmp <epilogue>; nop` 岛
    // ⇒ 裸早返形复现（if(mgr==0) return;）。
    if (mgr == 0)
        return;
    mgr->ConfirmSkillReq(static_cast<CUser*>(param.m_user));
}

// ---- respond_special_item_skill_tree_style —— ORIG 0x0827ce7e ----
// 置技能树 index 0 → UpdateData → InitSkill(user, true, kind=1)；组包
// (header 0/2, byte 0, short 1, make_basic_info, finalize) 后 Send。
#include "CCeraShop_Special/respond_special_item_skill_tree_style.inc"
                                                               const STSpecailItem_Param& param,
                                                               STSpecailItem_Result&)
{
    // b28d-D42 P28：ORIG 0x0827ce7e 先取 mgr（-0x10 槽）后取 user（-0xc 槽）。
    CPremiumLetheManager* mgr = G_CGameManager()->GetPremiumLetheManager();
    CUser* user = static_cast<CUser*>(param.m_user);
    // b28d-D42 P227：ORIG 0x0827ce7e 区 edi/ebx 缓存两变参值（先于 ctor）⇒
    // 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 499, 0)(
        "SKILL_TREE_STYLE : BUY_SKILL_TREE_STYLE, char(%s), char_no(%d)",
        user->getCurCharacName(), user->getCurCharacNo());
    user->SetCurCharacSkillTreeIndex(0);
    user->UpdateData();
    mgr->InitSkill(user, true, (ENUM_SKILL_TREE_KIND)1);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    user->Send(packet);
}

// ---- respond_special_item_booster_selection —— ORIG 0x0827ccfc ----
// 转发 CCeraShop::_processBoosterSelect（参数映射见报告汇编）。
#include "CCeraShop_Special/respond_special_item_booster_selection.inc"
    int, const STSpecailItem_Param& param, STSpecailItem_Result& result)
{
    // b28d-solo-CCeraShop_Special verdict=DIFF [N82] 63/63 等长；P0 九具名局部复现
    // ORIG 槽序（-0x34..-0x19），P167 CLEAR 1/1，余帧 0x5c/0x50 + m_flag bool 归一化。
    // b28d-solo(P0)：ORIG 0x0827ccfc 九个实参各自占具名栈槽（-0x34..-0x19，
    // 逐字段 `mov 0x10(%ebp),%eax; mov disp(%eax),%eax; mov %eax,-N(%ebp)`），
    // 声明序 = ORIG 槽序 user/itemIdx/items/ceraParam/count/time/flag/c1/c2。
    // [L14] 判据⑩ 攻法实测（回退）：头声明 `char framePad[0x10]` 使帧 0x50→0x60
    // （P232 生效），但九个具名标量槽号仍 -0x24..-0x9 **不动**（4.4 把数组 pad
    // 另置，不入标量块）⇒ 整块平移不可由 pad 修正，判 N82附。
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items =
        static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* ceraParam =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    int count = param.m_count;
    long time = param.m_time;
    // [L14] ORIG 0x0827cd3d `mov 0x18(%eax),%eax; mov %al,-0x1b(%ebp)`（直存字节、
    // 无 test/setne 归一化）⇒ 原码该局部是 **unsigned char**（bool 局部会发 setne）。
    // 传参处 ORIG 0x0827cdxx `movzbl -0x1b(%ebp),%ebx` 与 uchar 直传一致。
    unsigned char flag = param.m_flag;
    char c1 = static_cast<char>(param.m_char1d);
    char c2 = static_cast<char>(param.m_char1e);
    m_owner->_processBoosterSelect(user, itemIdx, count, result.m_value, time,
                                   flag, *items, *ceraParam, c1, c2);
}

// ---- respond_special_item_cerapoint_giftcard —— ORIG 0x0827de4e ----
// ORIG：经 GlobalData::s_pIPGHelper 虚表 +0xc 调用
//   (helper, user, value, 2, 0, 0)。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P254 enum 视图 STSpecailItem_ParamValueEnumView 复现两段装载
#include "CCeraShop_Special/respond_special_item_cerapoint_giftcard.inc"
    int, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    // ORIG 0x827de4e：vtable+0xc 载入→%ebx 跨实参求值存活（P52 单表达式形）。
    // IPGHelper 该槽未建模，保留裸槽派发。
    // [N7 封顶] 单表达式后 4.4 分配 vtable 项→%eax、value→%ebx、user→%ecx，
    // ORIG 为 ebx/ecx/edx——纯寄存器名互换（N7 不可翻）；帧 0x24+push%ebx 已复现。
    // b28d-D42 P265：ORIG 0x0827de4e 的 param.m_value 装载为两段
    // `mov 0x8(%eax),%eax; mov %eax,%ecx`——按 P265 在实参处显式 cast 到 enum
    // 复现该中转（int 字段 + enum 实参 ⇒ 4.4 保留 eax 中转）。
    (*reinterpret_cast<void (***)(void*, void*, int, int, int, int)>(
        GlobalData::s_pIPGHelper))[3](
        GlobalData::s_pIPGHelper,
        CERA_PARAM_VALUE_ENUM(param)->m_user,
        CERA_PARAM_VALUE_ENUM(param)->m_value,
        2, 0, 0);
}

// ---- respond_special_item_charac_slot_extend —— ORIG 0x0827c0c2 ----
// 增角色槽限 → 组 SIG_EXTEND_CHARAC_SLOT 包（acc_id @0 / slotEffect @4 /
// slotLimit @5）入队列 2 → _sendCharacSlotExtendPacket。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P82 具名视图 + P211 字段同型化（LHS-first）
#include "CCeraShop_Special/respond_special_item_charac_slot_extend.inc"
    int type, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    // b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827c0c2 orig/ours=92/92 ndiff=64。
    // 残差为帧/槽号平移 + CStreamGuard/STL 内联差；语义按报告逐条对齐。
    CERA_PARAM_TYPED(param)->m_user->incCharacSlotLimit(
        static_cast<unsigned char>(CERA_PARAM_TYPED(param)->m_value));

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0x8d),
                           true);
    guard.operator*()->operator<<((int)0x28a);
    guard.operator*()->operator<<(-1);
    SlotExtendPayloadView* buf = reinterpret_cast<SlotExtendPayloadView*>(
        guard->GetInBuffer<SIG_EXTEND_CHARAC_SLOT>());
    buf->m_accId =
        static_cast<int>(CERA_PARAM_TYPED(param)->m_user->get_acc_id());
    buf->m_slotEffect = static_cast<char>(
        CERA_PARAM_TYPED(param)->m_user->getSlotEffectCount());
    buf->m_slotLimit = static_cast<char>(
        CERA_PARAM_TYPED(param)->m_user->getCharacSlotLimit());
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    m_owner->_sendCharacSlotExtendPacket(CERA_PARAM_TYPED(param)->m_user, type);
}

// ---- respond_special_item_inventory_upgrade —— ORIG 0x0827bf90 ----
// 背包容量 < 0x10 时：ExpandInventory 日志 → 容量 += value → DB_UpdateInventoryCapacity
// 持久化 → _sendInventoryUpgradePacket；否则 result.m_value = 0x12。
// b28d-solo-CCeraShop_Special verdict=NEAR [N7] 92/92 等长，仅 makeRequest 三 callee-saved 寄存器角色轮换
#include "CCeraShop_Special/respond_special_item_inventory_upgrade.inc"
    int type, const STSpecailItem_Param& param, STSpecailItem_Result& result)
{
    // b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827bf90 orig/ours=92/92 ndiff=62。
    // 残差为帧/槽号平移 + 日志/STL 内联差；语义按报告逐条对齐。
    CUser* user = static_cast<CUser*>(param.m_user);
    // ORIG 0x827bf90..：getCurCharacInvenR（const 版，无 enableSaveInven 副作用）
    // 与 get_inventory_capacity 各 5+ 处逐点重发（P48/P183 禁缓存）。
    if (user->getCurCharacInvenR()->get_inventory_capacity() > 0xf)
    {
        result.m_value = 0x12;
        return;
    }
    user->m_historyLog.ExpandInventory(
        user->getCurCharacInvenR()->get_inventory_capacity(),
        user->getCurCharacInvenR()->get_inventory_capacity() + param.m_value, 0);
    // ORIG set 处 this 仍是第 5 次 getCurCharacInvenR（const 视图强转，
    // 全函数 InvenR×6 / capacity×5 静态调用点，无 W 版）。
    const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(
        user->getCurCharacInvenR()->get_inventory_capacity() + param.m_value);
    DB_UpdateInventoryCapacity::makeRequest(
        user->GetServerGroup(), user->GetUID(),
        user->getCurCharacNo(),
        user->getCurCharacInvenR()->get_inventory_capacity());
    m_owner->_sendInventoryUpgradePacket(CERA_PARAM_TYPED(param)->m_user, type);
}

// ---- respond_special_item_booster —— ORIG 0x0827ca4a ----
// setOriginalItem → _onBuySpecialItem(user, 6, itemIdx) → 遍历 find_item(itemIdx)
// 的 booster 子物品表 getBoosterItem，逐项 _processAdditionalItem →
// GetRestrictCode/IsRestrictedGoods/SetRestrictedGoods/SaveFeaturedIdx → SaveInventory。
// 子物品项 vtable+0x60 槽经 CItem 虚表序核对 = set_parent_booster_item_id(ulong)，
// 已按真实虚方法直调（CItem.h 声明，槽位 +0x60 吻合）。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 205/201；P167 CLEAR 29/29
// （const_iterator 修法消 5 ptEv+2 ne/ppEv 差集），余槽号/寄存器分配。
#include "CCeraShop_Special/respond_special_item_booster.inc"
                                                      const STSpecailItem_Param& param,
                                                      STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items = static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* cera =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    // ORIG 0x0827ca76 `mov 0x14(%eax),%eax; mov %eax,-0x2c(%ebp)`：count 读入槽后
    // 不再被读（-0x2c 随后被 restrictCode 覆盖）——4.1 保留的死读取（P171）。
    int count = param.m_count;
    (void)count;
    // ORIG 0x0827ca7f：time 具名槽 -0x28（收尾 SaveFeaturedIdx 第 3 实参直读槽）。
    long time = param.m_time;

    m_owner->setOriginalItem(itemIdx);
    m_owner->_onBuySpecialItem(user, ENUM_CERA_SPECIALITEM_BOOSTER,
                               static_cast<unsigned long>(itemIdx));

    // ORIG 0x0827cabf `movl $0x0,-0x24(%ebp)`：写后不读的死局部（P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;

    CStackableItem* base = static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    std::vector<stStackableBoosterItemInfo_t> boosterList;
    // ORIG 0x0827caf5：find_item 结果直调 getBoosterItem（无判空 cmpl/je）。
    base->getBoosterItem(boosterList);

    // b28d-solo(P167 判据)：ORIG 0x0827ca4a 用 const_iterator（ne/ppEv/operator-> 全
    // const 版 + iterator→const_iterator 转换构造 ×2）；非 const 版多发/少发上述符号。
    for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it = boosterList.begin(),
             itEnd = boosterList.end();
         it != itEnd; ++it)
    {
        // 实测：帧仍 0x8c（未跨档）、报告 diff 124/124→117/119、our 205→203 ⇒
        // 保留（不劣化且收敛）；余 1 槽待后续定位。
        // [G10 实验·P231/P52] ORIG 浅区槽比本形少 1（我方多 0x40）⇒ subItem 跨
        // set_parent_booster_item_id 虚调保活，试纯 register（P0 合规、非 asm 绑定）。
        register CItem* subItem = G_CDataManager()->find_item((int)it->m_itemIdx);
        // ORIG vtable+0x60 = CStackableItem::set_parent_booster_item_id(unsigned long)
        if (subItem != 0)
            subItem->set_parent_booster_item_id((unsigned long)itemIdx);
        // _processAdditionalItem 实参：itemIdx, addInfo(+0x8), avatarLevel(+0xe), avatarType(+0xd)
        m_owner->_processAdditionalItem(user, (unsigned long)it->m_itemIdx, it->m_addInfo,
                                        it->m_avatarLevel, it->m_avatarType, *items, *cera, 0);
        if (subItem != 0)
            subItem->set_parent_booster_item_id(0);
    }

    int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
    if (restrictCode != 0)
    {
        if (!user->IsRestrictedGoods(restrictCode))
        {
            user->SetRestrictedGoods(restrictCode);
            m_owner->SaveFeaturedIdx(user, restrictCode, (int)time, 0);
        }
    }
    user->SaveInventory();
}

// ---- respond_special_item_package —— ORIG 0x0827c204 ----
// 完整还原。此前 BLOCKED 判定的"匿名 static helper 0x827e06c..0x827e320"实为 STL 模板实例
// （std::vector<tagPackageItemSelection> / std::vector<STStackablePackageData> / iterator 操作），
// 由编译器依本函数 STL 用法自动生成，无需手写。依赖的 CCeraShop 方法
// (_onBuySpecialItem/_processAdditionalItem) 声明于 CCeraShop.h，实现由 CCeraShop 还原批次提供。
// 包字段/随机箱统计调用链：CStackableItem::m_pkg21c(+0x21c) 包列表、m_pkg234(+0x234)
// 逐选择包表，ceraParam 的 vector<tagPackageItemSelection>(+0xc)/vector<pair<int,int>>(+0x0)，
// 逐包 _processAdditionalItem（子物品 vtable+0x60 槽按 CItem::set_parent_booster_item_id
// 真实虚方法直调）+ CUser::SendRandomBoxStatistic(0x380/0x28982d/0x289749)。
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827c204 orig/ours=557/547 ndiff=497。
// 巨件帧/槽平移 + tagPackageItemSelection STL 内联差（N89）；P167 调用点已核。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82/N89] 557/547；P167 CLEAR 90/90，余 STL 内联/帧槽（§6.2 语义完成封顶）
// b28d-sweepCCeraShop（本轮）557/557 gap=0：①switch 三 case 按 ORIG 块址序重排（P324，
// 见下）；②两处 error 日志的 `accStr` 具名局部删去、内联回实参位（P329，复现 ORIG
// characNo→ebx 跨调用保活）；③守卫改德摩根正极性 `outer>=size || inner>=size`（P258，
// 复现 ORIG `jae→mov $1 / jb→mov $0` 失败位物化）。剩余残差 = 槽号分配（ORIG deadSlot
// 独立 -0x34 vs 4.4 复用 count 槽 -0x40，N82 不复用）+ 尾部 1 条对齐 nop（§6.1 豁免），
// opcode 多重集差异仅此 nop；p167 CLEAR、虚槽 vtable+0x60 两侧一致。
// + ④pkgTable 声明移入 `if(!m_sel.empty())` 块内（ORIG 0x0827c3c9 在 empty() 检查后
// 赋值，P0 槽序）；⑤pairs 循环缓存 `pitEnd`（P13：ORIG 0x0827c7bd 循环外 begin/end
// 各一次，我方原在条件里重发 end()）⇒ 修后 **jcc 目标下标与 ORIG 逐位全等（32/32）**、
// 同长函数 jcc 目标比对全库 0 不匹配（Main 高优先级排查）。
#include "CCeraShop_Special/respond_special_item_package.inc"
    int, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items = static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* cera =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    // ORIG 0x0827c236 `mov 0x14(%eax),%eax; mov %eax,-0x3c(%ebp)`：count 读入槽后
    // 不再被读（-0x3c 随后被 restrictCode 覆盖）——4.1 保留的死读取（P171）。
    int count = param.m_count;
    (void)count;
    long time = param.m_time;

    if (itemIdx == 0x1b)
    {
        // 疯狂升级事件包：header 0xb4 + body "0612\0"+accId+characNo 入队列 4。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0xc3),
                               true);
        guard.operator*()->operator<<((int)0xb4);
        guard.operator*()->operator<<(-1);
        // ORIG 0x0827c2f5/0x0827c30e 为 `mov -0x2c(%ebp),%edx; mov %eax,0x5(%edx)`
        // 具名结构字段写（裸指针 buf+5 会退化成 lea+间接 store）⇒ 用已有
        // SaveCrazyLevelupPayloadView 具名成员（P82/P211）。
        SaveCrazyLevelupPayloadView* buf = reinterpret_cast<SaveCrazyLevelupPayloadView*>(
            guard->GetInBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
        memcpy(buf, "0612\0", 5);                       // DAT_08c07e5a
        buf->m_accId = user->get_acc_id();
        buf->m_characNo = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(4), guard);
    }

    m_owner->_onBuySpecialItem(user, (ENUM_CERA_SPECIALITEM)5,
                               static_cast<unsigned long>(itemIdx));

    // ORIG 0x0827c37b `movl $0x0,-0x34(%ebp)`：写后不读的死局部（P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;

    // ORIG 实证元素类型（respond_special_item_package 反编译）：
    //   +0x21c = std::vector<STStackablePackageData>（包基础物表）
    //   +0x234 = std::vector<std::vector<STStackablePackageData>>（每选择包组表）
    // 头文件 m_pkg21c/m_pkg234 当前为 vector<char> 占位（元素类型修正涉共享头
    // CStackableItem.h 的多 TU 验证，另行批次）；此处以具名字段锚定偏移、按
    // ORIG 类型取视图。
    CStackableItem* stackItem =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    std::vector<STStackablePackageData> pkgList(
        *reinterpret_cast<std::vector<STStackablePackageData>*>(&stackItem->m_pkg21c));
    // ORIG 无 selections/pairs 具名引用槽：每处按 `mov -0x40(%ebp),%eax;
    // lea 0xc(%eax),%edx`（4 次）重算 cera->m_sel / cera->m_pairs（P48 无 CSE）。
    if (!cera->m_sel.empty())
    {
        // ORIG 0x0827c3c9..0x0827c3d1：pkgTable 赋值在 `m_sel.empty()` 检查
        // **之后**（index 128-130）⇒ 原码声明在 if 块内，非函数级（P0 槽序）。
        // ORIG operator[] 为 const 版（local_2c const 视图）+ inner.at() const。
        const std::vector<std::vector<STStackablePackageData> >* pkgTable =
            reinterpret_cast<const std::vector<std::vector<STStackablePackageData> >*>(
                &stackItem->m_pkg234);
        if (cera->m_sel.size() > 1)
        {
            // 多选择：逐项记录 error 日志（kdd372418Log 前缀，ORIG 原样）。
            for (std::vector<tagPackageItemSelection>::iterator it = cera->m_sel.begin();
                 it != cera->m_sel.end(); ++it)
            {
                // P329：ORIG 0x0827c5da..0x0827c605 右到左求值（characNo 先于
                // NumberToString），accStr 无具名槽 ⇒ 表达式内联回实参位。
                LogManager::logFormat(
                    1, "SpecialItemHandler.cpp",
                    __PRETTY_FUNCTION__,
                    0xdb, "kdd372418Log::Package Selection item error (%s,%d,%d,%d,%d)",
                    NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1),
                    it->m_itemIdx, (int)it->m_outer, (int)it->m_inner);
            }
        }
        // 主选择循环：找 itemIdx 匹配项，校验 pkgTable 索引，push 到 pkgList 并 erase。
        for (std::vector<tagPackageItemSelection>::iterator it = cera->m_sel.begin();
             it != cera->m_sel.end(); )
        {
            if (it->m_itemIdx == itemIdx)
            {
                // ORIG 0x0827c53a..0x0827c59d：`jae→mov $1 / jb→mov $0 / test;je 成功`
                // = 失败位 1 的德摩根物化（P258：`A||B` 形而非 `!(a&&b)`）⇒ 写
                // 正极性 `outer>=size || inner>=size` 失败臂，成功臂直落。
                if (it->m_outer >= pkgTable->size() ||
                    it->m_inner >= (*pkgTable)[it->m_outer].size())
                {
                    // P329：右到左内联求值（ORIG 0x0827c5e8/0x0827c5ef）。
                    LogManager::logFormat(
                        1, "SpecialItemHandler.cpp",
                        __PRETTY_FUNCTION__,
                        0xe7, "Package Selection error (%s,%d,%d,%d,%d)",
                        NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1),
                        it->m_itemIdx, (int)it->m_outer, (int)it->m_inner);
                    break;
                }
                pkgList.push_back((*pkgTable)[it->m_outer].at(it->m_inner));
                it = cera->m_sel.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // 逐包处理：配对 itemIdx→count 消费、set_parent_booster_item_id（vtable+0x60 槽）、
    // _processAdditionalItem、SendRandomBoxStatistic。
    // ORIG：pkgList 循环用 const_iterator 对（begin/end 各一次转换构造，
    // it-> 逐点重发 operator->const，P157/P183）；pairs 消费前先 empty() 短路。
    std::vector<STStackablePackageData>::const_iterator itBeg = pkgList.begin();
    std::vector<STStackablePackageData>::const_iterator itEnd = pkgList.end();
    for (; itBeg != itEnd; ++itBeg)
    {
        unsigned char local25 = 0;
        if (!cera->m_pairs.empty())
        {
            // P13：ORIG 0x0827c7bd..0x0827c7d5 在循环**外**各求一次 begin()/end()
            // （sret -0x88/-0x8c），循环条件只做 operator!=；我方原形在条件里
            // 每次重发 end() ⇒ 循环入口块错位。缓存 pitEnd。
            std::vector<std::pair<int, int> >::iterator pit = cera->m_pairs.begin();
            std::vector<std::pair<int, int> >::iterator pitEnd = cera->m_pairs.end();
            for (; pit != pitEnd; )
            {
                if (pit->first == itBeg->m_field0)
                {
                    local25 = static_cast<unsigned char>(pit->second);
                    cera->m_pairs.erase(pit);
                    break;
                }
                else
                {
                    ++pit;
                }
            }
        }

        CItem* sub = G_CDataManager()->find_item(itBeg->m_field0);
        // ORIG (**(code**)(*sub + 0x60))(sub, itemIdx)：CItem vtable 序核对槽 +0x60
        // = set_parent_booster_item_id(unsigned long)，按真实虚方法直调。
        if (sub != 0)
            sub->set_parent_booster_item_id(static_cast<unsigned long>(itemIdx));

        m_owner->_processAdditionalItem(
            user, static_cast<unsigned long>(itBeg->m_field0),
            static_cast<unsigned short>(itBeg->m_field4), local25, 0xff, *items, *cera, 0);

        if (sub != 0)
            sub->set_parent_booster_item_id(0);

        // 随机箱统计调用链（ORIG 0x0827c8fc..0x0827c976）：`movl $0x0,-0x1c(%ebp)`
        // + 单次 operator-> 取值 + 顺序 cmp 链（switch 形）；每个 case 把常量赋回
        // 具名 statKind 槽后传参（`movl $N,-0x1c; mov -0x1c,%eax; mov %eax,(%esp)`）。
        // P324：case 叶块物理序 = 书写序；ORIG 块序 0x380(+0x720)→0x28982d(+0x73d)
        // →0x289749(+0x759)，末块 fall through 到循环增量（无 jmp）⇒ 按块址序书写。
        int statKind = 0;
        switch (itBeg->m_field0)
        {
        case 0x380:
            statKind = 0;
            user->SendRandomBoxStatistic(statKind, (ENUM_RANDOMBOX_ACTION_KIND)0);
            break;
        case 0x28982d:
            statKind = 1;
            user->SendRandomBoxStatistic(statKind, (ENUM_RANDOMBOX_ACTION_KIND)0);
            break;
        case 0x289749:
            statKind = 2;
            user->SendRandomBoxStatistic(statKind, (ENUM_RANDOMBOX_ACTION_KIND)0);
            break;
        }
    }

    // 收尾：GetRestrictCode → SetRestrictedGoods/SaveFeaturedIdx → SaveInventory。
    int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
    if (restrictCode != 0)
    {
        if (!user->IsRestrictedGoods(restrictCode))
        {
            user->SetRestrictedGoods(restrictCode);
            m_owner->SaveFeaturedIdx(user, restrictCode, static_cast<int>(time), 0);
        }
    }
    user->SaveInventory();
}

// ---- respond_special_item_premium —— ORIG 0x0827d3dc ----
// g_map_premium_item_data[itemIdx] 命中后按 premium type (STPremiumItemData.m_value0)
// jump-table 分发：term/flag/item 取自 m_value4(低2字节)/m_value8。各 case 处理
// 限制码/日志/技能备份/表情/疲劳/金币等；收尾统一走
// cal_end_time → send_db → handleSetUserPremium → handlePremiumEffect →
// is_notify/_sendPremiumPacket → write_log。未命中记 PREMIUM_ERROR 日志返回。
// 注：报告汇编中 "ItemVendingMachine::BuyAuctionItem" 标签为 report_resolve 误标，
// 实际地址 0x0817a10c 为 CItemList::GetRestrictCode（nm 核实），本函数不使用 BuyAuctionItem。
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827d3dc orig/ours=672/703 ndiff=680。
// 巨件帧/槽平移 + STL/字符串内联差（N89）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82/N89] 672/703 ndiff 890→863；lethe
// 获取前置到 param 字段读取之前（ORIG 0x0827d3dc 序）；P167 CLEAR 101/101；余帧
// 0xc0/0xf4 + 槽平移封 N82。
#include "CCeraShop_Special/respond_special_item_premium.inc"
                                                      const STSpecailItem_Param& param,
                                                      STSpecailItem_Result&)
{
    // ORIG 0x0827d3dc+：lethe 管理器函数头单次获取入槽（local_48），先于
    // param 字段读取。
    CPremiumLetheManager* lethe =
        G_CGameManager()->GetPremiumLetheManager();

    CUser* user = static_cast<CUser*>(param.m_user);
    // [G10 实验] ORIG 0x0827d3f7..0x0827d405：`mov 0x4(%eax),%eax; mov %eax,-0x9c(%ebp)`
    // 单条 4B store 后 **直接** `lea -0x9c(%ebp),%edx` 作 map::find 的 key 地址
    // （0x0827d43c）⇒ 该局部与 map key 同型（32 位下 unsigned long=4B），
    // 无 int→ulong 转换临时槽。旧 `int itemIdx` + 显式 cast 使 4.4 多出
    // 转换临时（我方 -0x48→-0xa8 两次 store）。
    // 实测（fresh .o，同一 ORIG 基线）：diff 段 +538/-519（3 hunks）→
    // +524/-501（3 hunks）；TU 四数 176/319(合计 495)/3/11/104 逐值不变 ⇒ 不劣
    // ⇒ 保留（帧 0xd0 未变，槽号由 -0x50/-0x4c... 上移到 -0x4c/-0x48...，
    // 与 ORIG -0x44/-0x40... 之差收敛到 8 = P332 版本首槽常数）。
    unsigned long itemIdx = param.m_itemIdx;
    int count = param.m_count;
    // ORIG 0x0827d414：`movzbl 0x1c(%eax),%eax; mov %al,-0x35` —— flag 是**字节槽**
    // （后续 9 处 `movb $0x1,-0x35`、1 处 `movzbl -0x35,%eax`）⇒ 原码为 unsigned char。
    unsigned char flag = param.m_flag;          // -0x35
    // ORIG 槽：timeVal@-0x34（SaveFeaturedIdx 用，case 0xd 不改）、timeA@-0x30（base）、
    // start@-0xa0（cal_end_time 的 start 出参，初值 timeA，0827d430）、end@-0xa8。
    long timeVal = param.m_time;                // -0x34
    long timeA = timeVal;                       // -0x30
    long start = timeA;                         // -0xa0

    std::map<unsigned long, STPremiumItemData>::iterator it =
        g_map_premium_item_data.find(itemIdx);
    if (it == g_map_premium_item_data.end())
    {
        LogManager::logFormat(
            1, "SpecialItemHandler.cpp",
            __PRETTY_FUNCTION__,
            0x2ff, "PREMIUM_ERROR : NO PREMIUM ITEM!! : ITEM_ID (%u)",
            (unsigned int)itemIdx);
        return;
    }

    // ORIG 四字段各发一次 _Rb_tree_iterator::operator->（P157 禁缓存引用）。
    int ptype = it->second.m_value0;            // 商品类型（jump 键）
    int term = (int)it->second.m_c4;            // 期限（signed char）
    unsigned int flag2 =
        (unsigned int)(unsigned char)it->second.m_c5;  // 次字节
    int item = it->second.m_value8;             // 限制物品 idx
    int resultVal = premium_helper_function::cal_premium_term(
        term, param.m_char1f != 0, param.m_extra);


    switch (ptype)
    {
    case 8:
        flag = 1;
        break;
    case 0xf:
        if (itemIdx == 200)
            resultVal = 0x1e;
        else if (itemIdx == 0xcb)
            resultVal = 3;
        else if (itemIdx == 0xcc)
            resultVal = 7;
        else if (itemIdx == 0xcd)
            resultVal = 0xe;
        if (user->getCurCharacInvenR()->GetCoin() < 3)
        {
            // ORIG 0x0827d5f2：三次 getter 结果经 %ebx 累加（无 coin/payCoin 具名槽）
            // ⇒ 单表达式（P0/P157：每次重新取得 invenR）。
            int sum = user->getCurCharacInvenR()->GetCoin() +
                      user->getCurCharacInvenR()->GetPayCoin() +
                      user->getCurCharacInvenR()->GetEventCoin();
            int amt = 3;
            if (100000 < sum + amt)
                amt = 100000 - sum;
            if (0 < amt)
            {
                user->getCurCharacInvenW()->UpdateDailyCoin(amt);
                user->m_historyLog.CoinAdd(
                    user->getCurCharacInvenR()->GetCoin(), amt,
                    (eCoinAddReason)4);
            }
        }
        break;
    // ORIG 0x16/0x1b/0x1d 三 case 各含独立同形调用块（-O0 不合并，×3 静态点）。
    case 1:
    case 3:
        resultVal = WongWork::CEventMgr::GetPremiumServiceBonus() + resultVal + 1;
        break;
    case 0xd:
    {
        // ORIG 0x0827d6ea：短路结果零槽物化（mov$1/jmp/mov$0/test;je，值只在 %eax）
        // ⇒ P226 扩展：语句表达式包住整条条件（内不含具名变量）。
        if (({ user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xd) != 1 &&
                user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xe) == 1; }))
        {
            WongWork::SUserPremiumInfo* info = user->GetPremiumInfo()->GetPremiumInfo(0xe);
            if (info->m_type == 0xe)
            {
                // ORIG 0x0827d730/733/736：只更新 timeA(-0x30) 与 start(-0xa0)，
                // 不动 timeVal(-0x34)（SaveFeaturedIdx 仍用原始 param.m_time）。
                timeA = info->m_end;
                start = timeA;
            }
        }
        break;
    }
    case 0xe:
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xd) != 0)
        {
            count = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
            ptype = 0xd;
        }
        break;
    case 0x16:
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        break;
    case 0x1b:
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        break;
    case 0x1d:
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        break;
    case 0x21:
    {
        if (flag2 == 0 && count != 0 &&
            !user->IsRestrictedGoods(count))
        {
            user->SetRestrictedGoods(count);
            int serverCode = 0;
            if (g_SPremiumInfo[ptype].m_useTargetServer == 1)
                serverCode = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378
            m_owner->SaveFeaturedIdx(user, count, (int)timeVal, serverCode);
        }
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        // ORIG 0x0827d8ac：varargs（no→%esi、name→%ebx）在 cMyTrace ctor 之前求值并跨
        // ctor 保活 ⇒ P227 全表达式临时链（禁具名临时）。
        cMyTrace(__PRETTY_FUNCTION__, 0x3a5, 0)(
            "ONE_DAY_LETHE : BUY_RESTRICTED_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        // ORIG：lethe 判空用函数头 local_48 槽；SkillSave 两条各自重发
        // get_charac_job/getCurCharacSkillR（P48），技能块 = SkillR 视图 +0x46/
        // +0x1de（SkillSlot.h m_skillSlot/m_skillSlot2 权威偏移，直加数形态）。
        if (lethe != 0)
        {
            if (user->IsEndLetheAtLogin() != 1)
            {
                lethe->BackupSkill(user, SKILL_TREE_1);
                lethe->BackupSkill(user, SKILL_TREE_2);
            }
            user->m_historyLog.SkillSPInfoSave(user, (eSkillSPInfoSaveReason)0);
            user->m_historyLog.SkillSave(
                reinterpret_cast<const _Mastered_skill*>(
                    user->getCurCharacSkillR() + 0x46),
                (unsigned int)user->get_charac_job(), SKILL_TREE_1,
                (eSkillSaveReason)0);
            user->m_historyLog.SkillSave(
                reinterpret_cast<const _Mastered_skill*>(
                    user->getCurCharacSkillR() + 0x1de),
                (unsigned int)user->get_charac_job(), SKILL_TREE_2,
                (eSkillSaveReason)0);
        }
        break;
    }
    case 0x4f:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3cb, 0)(
            "GROWTH_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        break;
    }
    case 0x53:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3d3, 0)(
            "EMOTICON_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x57) != 0)
            m_owner->_ClosePremium(user, 0x57);
        break;
    }
    case 0x57:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3db, 0)(
            "EMOTICON_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x53) != 0)
            m_owner->_ClosePremium(user, 0x53);
        break;
    }
    case 0x5c:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3e5, 0)(
            "CUBE_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        break;
    }
    default:
        // ORIG default：count==0 时按 itemIdx 现取 restrict code。
        if (count == 0)
{
            count = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
    }
        break;
    }

    // ---- 公共收尾（switch 之后）----
    if (count != 0 && user->IsRestrictedGoods(count) != 1)
    {
        user->SetRestrictedGoods(count);
        int ts = premium_helper_function::get_target_server(ptype);
        m_owner->SaveFeaturedIdx(user, count, (int)timeVal, ts);
    }

    bool needGain = false;
    if (g_SPremiumInfo[ptype].m_needDailyCheck == 0 ||
        !user->CheckFatigue())
        needGain = false;
    else
        needGain = true;
    if (needGain)
        user->gainCurCharacFatigue(1);

    long end = 0;
    premium_helper_function::cal_end_time(user, start, end, timeA, ptype,
                                          flag != 0, resultVal);
    premium_helper_function::send_db(user, ptype, start, end, count);
    WongWork::CHandlePremium::handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)ptype,
                                                   start, end, count, true);
    WongWork::CHandlePremium::handlePremiumEffect(user, (ENUM_PREMIUM_TYPE)ptype);
    if (premium_helper_function::is_notify(user, ptype) != 1)
        m_owner->_sendPremiumPacket(user, type, ptype, (int)(end - timeA));
    premium_helper_function::write_log(user, ptype);
}

// ============================================================================
// premium_helper_function（免费命名空间）—— ORIG 0x827cff0
// ============================================================================
// if(flag) term *= (byte)CDataManager+0x877e；return term * extra。
#include "CCeraShop_Special/cal_premium_term.inc"
{
    if (flag)
        return (int)G_CDataManager()->m_boosterGage.m_cMultiboxCount * term * extra;
    return term * extra;
}

// ---- cal_end_time —— ORIG 0x0827d024 ----
// 计算 premium 开始/结束时间（param_6 时按既有 premium 结束续期）。
#include "CCeraShop_Special/cal_end_time.inc"
                                           long base, int type, bool bActive,
                                           int term)
{
    if (bActive)
    {
        // ORIG 0x827d03c：外层 GetPremiumInfo 返回经出参槽直传，仅内层结果
        // 具名槽 -0xc（P52/P0）。
        // b28c-L20: AE — 旧 [N 封顶] 两条判词均作废：① g_SPremiumInfo[type]
        // 访问现为单条 `mov <A>(%eax)`（全局 disp 折叠，与 ORIG 同形）；
        // ② 尾比较改书写序 limit 在左（见下）后 ORIG `cmp %eax,%edx; jle`
        // 逐条复现，非 N22。
        WongWork::SUserPremiumInfo* info =
            user->GetPremiumInfo()->GetPremiumInfo(type);
        if (info->m_type == type)
        {
            end = (long)info->m_end + (long)term * 0x15180;
            start = (long)info->m_start;
        }
        else
        {
            end = (long)term * 0x15180 + base;
        }
    }
    else
    {
        end = (long)term * 0x15180 + base;
    }
    // b28c-L20: AE — 尾比较 ORIG 0x827d0bf 为「RHS(limit) 先物化 edx、start 后载 eax、
    // `cmp %eax,%edx; jle`」= 源码书写序 limit 在左（探针 /tmp/l20_probe.cpp f2b：
    // `lim > start` 产 `cmp start,%eax; jle` 同族）；写 `start < limit` 4.4 恒
    // LHS 先载 + `jge`（旧 N22 判词作废）。
    if (g_SPremiumInfo[type].m_timeLimit != 0 &&
        g_SPremiumInfo[type].m_timeLimit > start)
    {
        end = (long)g_SPremiumInfo[type].m_timeLimit;
    }
}

// ---- get_target_server —— ORIG 0x0827d0f5 ----
// g_SPremiumInfo[type].m_useTargetServer(+0x5c)==1 时返回
// G_CEnvironment()->m_serverEnvir.m_serverGroup（CEnvironment+0x378），否则 0。
#include "CCeraShop_Special/get_target_server.inc"
{
    if (g_SPremiumInfo[type].m_useTargetServer == 1)
        return G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x198+0x1e0
    return 0;
}

// ---- send_db —— ORIG 0x0827d127 ----
// 投递 0x54 头 premium DB 流（uid/accId/type/start/end/flag/serverCode）至 DB_Q。
#include "CCeraShop_Special/send_db.inc"
                                      int flag)
{
    // ORIG 0x0827d0f5：acc_id/target_server 按无符号(lsEj)、start/end 按 long(lsEl) 写。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0x295),
                           true);
    guard.operator*()->operator<<((int)0x54);
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<((unsigned int)user->get_acc_id());
    guard.operator*()->operator<<((int)type);
    guard.operator*()->operator<<(start);
    guard.operator*()->operator<<(end);
    guard.operator*()->operator<<((int)flag);
    guard.operator*()->operator<<((unsigned int)get_target_server(type));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- is_notify —— ORIG 0x0827d29d ----
// premium 项存在且 m_state==2（等待开始）时返回 1。
#include "CCeraShop_Special/is_notify.inc"
{
    // ORIG 0x0827d29d：链式内联（GetPremiumInfo 结果不入槽）；m_type 槽直读
    // 单次，m_state==2 用 == 直比（三目会折 cmp 双物化，异形）。
    WongWork::SUserPremiumInfo* info =
        user->GetPremiumInfo()->GetPremiumInfo(type);
    if (info->m_type == type)
        return info->m_state == 2;
    return 0;
}

// ---- write_log —— ORIG 0x0827d2df ----
// type==0x53/0x57：重算+发表情；type==8：组 2 包并广播 basic_info。
// b28c-L20: AE — P17/P46：ORIG 0x827d2e7 为 switch 线性链（`mov 0xc(%ebp),%eax`
// 一次载入 + 三 `cmp $imm,%eax/je` 复用 + 两 je 同目标 + 尾 jmp）；if/else-if
// 在 4.4 生 `cmpl $imm,mem` 直比 + 第二条件 jne 落体（旧 N18a/b 判词作废）。
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

// ============================================================================
// checkBuyingCondition —— ORIG 0x08323f2a (T)
// 仅转发到 _CheckBuyingCondition(CUser*, uint, ulong, uint, ushort, uchar, ushort)。
// ============================================================================
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P292/P159：返回类型 int→bool
// 消 `movzbl %al,%eax`（ORIG 尾为 call;leave;ret 直透 bool 结果）
#include "CCeraShop_Special/checkBuyingCondition.inc"
                                     unsigned int b, unsigned short c, unsigned char d,
                                     unsigned short e)
{
    return _CheckBuyingCondition(user, a, itemIdx, b, c, d, e);
}

// ============================================================================
// _CheckConditionPutonAvatar —— ORIG 0x08324966 (T)
// 遍历角色装备槽（INVEN 0，slot 0..9）与头像槽（INVEN 2，slot 0..0x68），
// 任一槽位 Inven_Item 的 m_addInfo(+0x02) 非 0 即返回 1（已有装备/头像），否则 0。
// ============================================================================
// b28d-solo-CCeraShop_Special verdict=DIFF [P223 边界封顶] ORIG 0x08324966 56/54。
// ORIG 两守卫 je 分落 +0x4b(单独 nop 岛)/+0x4c(直接落 addl join)；4.4 岛数恒 =
// 条件数或 0（探针 fA/fB/hA/hB/hD/kA 六形全败：goto 形恒多发 `nop;jmp;nop`，
// 共享/空 else/continue 形恒折同目标）⇒ 保持 `&&` 原形（P48 双静态调用点）。
#include "CCeraShop_Special/_CheckConditionPutonAvatar.inc"
{
    // P48：两循环各自入口一次 getCurCharacInvenR 静态调用点（禁缓存）。
    for (int i = 0; i <= 9; ++i)
    {
        const Inven_Item* it = user->getCurCharacInvenR()->GetInvenRef(0, i);
        if (it != 0 && it->m_addInfo != 0)
            return 1;
    }
    for (int i = 0; i <= 0x68; ++i)
    {
        const Inven_Item* it = user->getCurCharacInvenR()->GetInvenRef(2, i);
        if (it != 0 && it->m_addInfo != 0)
            return 1;
    }
    return 0;
}

// ============================================================================
// _CheckBuyingCondition —— ORIG 0x08323f86 (T)
// 商城购买前置条件分发（uint/ulong 重载，被 checkBuyingCondition 与
// _CheckBuyingCondition(CCeraShopGoods*) 共用）。按 itemIdx 归一化 category
// 后分派 0x01/_CheckConditionPutonAvatar、0x03（创角时间窗）、0x04（仓库容量）、
// 0x05（背包容量+造物）、0x06（事件商品存在性）、0x08（等级）、0x0b（技能树）、
// 0x0c（角色槽/服务器环境）、0x0d（背包容量精确）、0x10（转职/可堆叠限定）。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08323f86 orig/ours=566/563 ndiff=530。
// 巨件帧/槽平移（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82/N89] 566/561 ndiff 357→351；
// P251/P286 switch(a) 复现无槽多路比较链（ORIG 0x0832482a `mov a,%eax; cmp; je`）；
// P167 CLEAR 52/52；余帧 0xc4/0xd4 + 槽平移封 N82。
#include "CCeraShop_Special/_CheckBuyingCondition.inc"
                                     unsigned int b, unsigned short c, unsigned char d,
                                     unsigned short e)
{
    // local_20：归一化 category。ORIG 先按 itemIdx 特例置 1/3，再按区间映射。
    int category = 0;

    // P251/P286：ORIG 0x0832482a `mov 0x10(%ebp),%eax; cmp $0x1adbd,%eax; je;
    // cmp $0x1c13c,%eax; je` 为无槽多路比较链 ⇒ 源形是 switch(a)，非 if-else。
    switch (a)
    {
    case 0x1adbd:
        category = 1;
        break;
    case 0x1c13c:
        category = 3;
        break;
    default:
        break;
    }

    // 真 bug 修复（①判据 + 报告 Ghidra C 648 行）：ORIG `if (param_3 != 1)`
    // 即持有**第 3 形参 itemIdx（ulong）**，旧码误用第 4 形参 b（uint）。
    if (itemIdx != 1)
    {
        // [G10 实验·P231/P52] ORIG 0x0832400x 无 citem 栈槽（浅区 store 集
        // ORIG-only 0x14/0x57；我方 0x20/0x24/0x28 三槽）⇒ 值以 callee-saved
        // 跨 IsAvatarItem 虚调保活。纯 register（非 asm 绑定，P0 合规）试形。
        register CItem* citem = G_CDataManager()->find_item((int)itemIdx);
        if (citem == 0)
            return 0;
        if (citem->IsAvatarItem())  // vtable +0x10（IsAvatarItem）
        {
            // ORIG 0x08324019：`e == 0 || getAvatarTypeSelect()->size() <= e` 短路，
            // 结果零槽物化（mov$1/mov$0/test），且 e==0 时跳过后一调用
            // ⇒ P226 语句表达式（含具名 bool 会落槽并多一对 store/load）。
            if (({ e == 0 ||
                   ((const std::vector<stAvatarTypeSelect_t>*)(((CEquipItem*)citem)
                        ->getAvatarTypeSelect()))->size() <= e; }))
                return 0;
            if (e != c)
                return 0;
            // 实测：帧 0xd4→0xc4（register citem）→0xc0；OURS-only 槽由 5 个降为 0；
            // 报告 diff 481/472→439/430；TU 四数 176/319/3/11/104 不变 ⇒ 保留。
            // [G10 实验·P0/P183/P157] ORIG 浅区 store 集只多 1 槽（0x14），我方多
            // 3 槽（0x20/0x24/0x28 = sel 指针 / selItem 引用 / slotCount）⇒ 三点全
            // 内联为单表达式（ORIG 每点重算 getAvatarTypeSelect()/at()，无 CSE）。
            if (c != 4 &&
                ((const std::vector<stAvatarTypeSelect_t>*)
                     (((CEquipItem*)citem)->getAvatarTypeSelect()))->at(e).m_field10 != 0)
                return 0;
        }
    }

    // 归一化 category（itemIdx 区间/特例映射，见报告汇编 0x0239..0x0241）。
    if (itemIdx < 0x34a)
    {
        if (itemIdx > 0x346)
            category = 0xb;
        else if (itemIdx < 0x44)
        {
            if (itemIdx < 0x3f)
            {
                if (itemIdx == 0x33)
                    category = 5;
                else if (itemIdx < 0x34)
                {
                    if (itemIdx == 0x32)
                        category = 4;
                }
                else if (itemIdx > 0x38)
                    category = 4;
            }
            else
                category = 6;
        }
        else if (itemIdx < 0xc1)
        {
            if (itemIdx < 0xbe)
            {
                if (itemIdx - 0x45 < 5)
                    category = 6;
            }
            else
                category = 0xb;
        }
        else if (itemIdx == 0x2e4 || itemIdx == 0x335)
            category = 0xb;
    }
    else
    {
        if (itemIdx > 0x28978c)
        {
            if (itemIdx < 0x2897ca)
            {
                if (itemIdx < 0x2897c8)
                {
                    if (itemIdx == 0x28978f)
                        category = 0xc;
                }
                else
                    category = 0xd;
            }
            else if (itemIdx > 0x28995c &&
                     (itemIdx < 0x28995f || itemIdx == 0x2899ab))
                category = 0x10;
        }
        else
        {
            if (itemIdx > 0x28978a)
                category = 4;
            else if (itemIdx == 0x1ee2 || itemIdx == 0x3a0)
                category = 0xb;
            else if (itemIdx > 0x1ee3)
            {
                if (itemIdx == 0x2896cb)
                    category = 0xb;
                else if (itemIdx == 0x28978a)
                    category = 5;
            }
        }
    }

    if (category == 0)
        return 1;

    // case 逻辑直接内联（ORIG 0x8323f86 为单一函数，无 file-static helper；
    // 各 case 代码形态/调用序对齐报告反编译 C）。
    switch (category)
    {
    case 1:
        return _CheckConditionPutonAvatar(user);

    case 3:
    {
        // ORIG 0x0832420f/0x08324219：函数内 static（nm 证实 .bss：
        // _ZZN8WongWork9CCeraShop21_CheckBuyingConditionEP5CUserjmjthtE14eventStartTime
        // @0x0943bec8 / ...E12eventEndTime @0x0943becc）**每次调用无条件重写**常量
        // ⇒ 原码为「声明无初值 + 赋值语句」（带初值声明会折进 .data 且无代码）。
        // [L14 轴 B（a_S->U 命中）] ORIG 0x08324236/0x0832424b 直证为
        // `mov <A>,%edx; cmp %edx,%eax; jl/jge`（**有符号序**）；
        // `getCurCharacCreateTime()` 返回 long（CUserCharacInfo.h:846），
        // 与 `unsigned int` 比较按 C++ 规则转无符号 ⇒ 我方出 `setb/setae`。
        // 改 `int` 复现有符号序（常量 0x4589a450/0x45ae8e50 均为正，值不变）。
        // [AxisB3 属主复核 2026-09-10 ⇒ **保留**] 地址自校验通过：
        //   `nm -S --defined-only ORIG` 得本函数 [0x08323f86, +0x889)，清单
        //   orig_addr 0x08324236/0x0832424b 均落在区间内；两侧上下文同构
        //   （`mov &eventStartTime/%edx; cmp %edx,%eax; jl/jge` ↔ 我方
        //   `mov 0x943bec8,%edx; cmp %edx,%eax; setl` —— 符号性已对齐）。
        //   实测：our 585→583（gap −11→−9，向 ORIG 574 收敛）；TU 四数
        //   176/319/3/11/104（identical/ae/near/diff/missing）逐值不变 ⇒ 不劣 ⇒ 保留。
        static int eventStartTime;
        static int eventEndTime;
        eventStartTime = 0x4589a450;
        eventEndTime = 0x45ae8e50;
        // ORIG 调用两次 getCurCharacCreateTime 后比较 [start,end) 窗口。
        if (user->getCurCharacCreateTime() < eventStartTime)
            break;
        if (user->getCurCharacCreateTime() >= eventEndTime)
            break;
        return 1;
    }

    case 4:
    {
        // 仓库容量精确匹配（0x8323f86 汇编 case4 容量表）。
        const int cap = user->getCurCharacCargoR()->GetCapacity();
        if (itemIdx == 0x3c)
        {
            if (cap == 0x48)
                return 1;
        }
        else if (itemIdx < 0x3d)
        {
            if (itemIdx == 0x39)
            {
                if (cap == 0x18)
                    return 1;
            }
            else if (itemIdx < 0x3a)
            {
                if (itemIdx == 0x32 && cap == 8)
                    return 1;
            }
            else if (itemIdx == 0x3a)
            {
                if (cap == 0x28)
                    return 1;
            }
            else if (itemIdx == 0x3b && cap == 0x38)
            {
                return 1;
            }
        }
        else if (itemIdx == 0x3e)
        {
            if (cap == 0x68)
                return 1;
        }
        else if (itemIdx < 0x3e)
        {
            if (cap == 0x58)
                return 1;
        }
        else if (itemIdx == 0x28978b)
        {
            if (cap == 0x78)
                return 1;
        }
        else if (itemIdx == 0x28978c && cap == 0x88)
        {
            return 1;
        }
        break;
    }

    case 5:
    {
        // 背包/仓库均无该物时 create_item 后 tryInsert 成功返回 1。
        // ORIG 0x8323f86 case5：InvenR×2（check_exist / tryInsert 两侧共两次静态
        // 调用点）、CargoR×1、check_item_exist×2；create_item 直访 m_itemList(+0xc)，
        // tryInsert 走 getCurCharacInvenR（const 视图，无 W 版）。
        // InvenR→check；失败侧再 CargoR→check（P48：结果存 bVar1，非缓存指针）。
        bool invClear = (user->getCurCharacInvenR()->check_item_exist((int)itemIdx) == -1);
        if (invClear)
        {
            bool cargoClear =
                (user->getCurCharacCargoR()->check_item_exist((int)itemIdx) == -1);
            if (cargoClear)
            {
                Inven_Item item;
                item.m_addInfo = (int)itemIdx;   // ORIG uStack_5b = param_3
                G_CDataManager()->m_itemList->create_item(
                    (ENUM_ITEM_CREATE_TYPE)0, item, 1);
                if (user->getCurCharacInvenR()->tryInsertItemIntoInventory(item) >= 0)
                    return 1;
            }
        }
        break;
    }

    case 6:
    {
        // 事件窗口内要求背包/仓库均无 0x65d6..0x65da 任一。
        // ORIG 双 getCurCharacCreateTime（start/end 各自求值）+ 全展开
        // 0x65d6..0x65da × (InvenR+check | CargoR+check)（P48/P183 逐点重发禁缓存；
        // ORIG 静态全展开五组，非循环——调用点差集据此）。
        if (user->getCurCharacCreateTime() < 0x46a7b9d0)
            break;
        if (user->getCurCharacCreateTime() >= 0x46d5de50)
            break;
        // 逐 id 短路链：任一 Inven/Cargo 持有 0x65d6..0x65da 中物品即失败。
        if (user->getCurCharacInvenR()->check_item_exist(0x65d6) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d6) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65d7) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d7) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65d8) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d8) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65d9) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65d9) == -1 &&
            user->getCurCharacInvenR()->check_item_exist(0x65da) == -1 &&
            user->getCurCharacCargoR()->check_item_exist(0x65da) == -1)
        {
            return 1;
        }
        break;
    }

    case 8:
        return user->get_charac_level() > 0xe ? 1 : 0;

    case 0xb:
        return user->GetCurCharacSkillTreeIndex() == -1 ? 1 : 0;

    case 0xc:
    {
        // ORIG 0x8323f86 case0xc：G_CEnvironment 两次（+0x1a8 serverType /
        // +0x378 serverGroup 各自取全局，P48 禁缓存）。
        if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
            return 0;
        const int grp = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        if (grp == 0xb || grp == 0xc || grp == 5)
            return 0;
        if (user->getCharacSlotLimit() < 0x15)
            return 1;
        break;
    }

    case 0xd:
    {
        const int cap = user->getCurCharacInvenR()->get_inventory_capacity();
        if (itemIdx == 0x2897c8)
        {
            if (cap == 0)
                return 1;
        }
        else if (itemIdx == 0x2897c9 && cap == 8)
        {
            return 1;
        }
        break;
    }

    case 0x10:
    {
        if (user->get_charac_job() == 9)
            return 0;
        if (user->get_charac_job() == 10)
            return 0;
        return user->verifyStackableItemActionPlace(
            StackableAction::TYPE_CLASS_CHANGE, (unsigned int)itemIdx);
    }
    }

    return 0;
}

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
{
    switch (itemIdx)
    {
    // P46：叶块物理顺序 = case 书写顺序；下表按 ORIG 0x08324eaf..0x0832570e 的
    // 块地址序书写（决策树与 case 序无关，故树形保持不变）。
    // P63/P257：ORIG 所有 case 共用尾出口 +0xe2f `mov $0x1,%eax` ⇒ 统一 goto。
    CERA_SPECIAL_CASE(110, 1, 0x28);
    CERA_SPECIAL_CASE(5, 1, 0x18);
    case 50:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 2660235:
    case 2660236:
        outType = 7;
        outValue = 0x10;
        goto L_true;
    CERA_SPECIAL_CASE(6, 1, 0x98);
    case 2660296:
    case 2660297:
        outType = 0xb;
        outValue = 8;
        goto L_true;
    CERA_SPECIAL_CASE(2660239, 8, 1);
    CERA_SPECIAL_CASE(2670009, 1, 0x28);
    CERA_SPECIAL_CASE(68, 1, 0x48);
    CERA_SPECIAL_CASE(9, 2, 1);
    CERA_SPECIAL_CASE(11, 2, 3);
    CERA_SPECIAL_CASE(16, 4, 8);
    CERA_SPECIAL_CASE(2675057, 4, 0x2a);
    CERA_SPECIAL_CASE(17, 3, 7);
    CERA_SPECIAL_CASE_F(18, 2, 9, true);
    CERA_SPECIAL_CASE_F(19, 2, 10, true);
    CERA_SPECIAL_CASE_F(20, 2, 0xb, true);
    CERA_SPECIAL_CASE_F(22, 2, 0xd, false);
    CERA_SPECIAL_CASE_F(23, 2, 0xe, false);
    CERA_SPECIAL_CASE_F(2675025, 2, 0x11, true);
    CERA_SPECIAL_CASE_F(30, 2, 0x11, true);
    CERA_SPECIAL_CASE_F(31, 2, 0x12, true);
    CERA_SPECIAL_CASE_F(32, 2, 0x13, true);
    CERA_SPECIAL_CASE_F(33, 2, 0x14, true);
    CERA_SPECIAL_CASE_F(34, 2, 0x15, true);
    CERA_SPECIAL_CASE_F(35, 2, 0x16, true);
    CERA_SPECIAL_CASE_F(741, 2, 0x24, true);
    CERA_SPECIAL_CASE_F(2660012, 2, 0x2d, true);
    CERA_SPECIAL_CASE_F(2660050, 2, 0x2f, true);
    CERA_SPECIAL_CASE_F(137, 2, 0x1d, true);
    CERA_SPECIAL_CASE_F(2675058, 2, 0x2b, true);
    CERA_SPECIAL_CASE_F(193, 2, 0x1e, true);
    CERA_SPECIAL_CASE_F(29, 2, 0x10, false);
    CERA_SPECIAL_CASE_F(2675026, 2, 0x17, true);
    CERA_SPECIAL_CASE_F(43, 2, 0x17, true);
    CERA_SPECIAL_CASE_F(44, 2, 0x18, true);
    CERA_SPECIAL_CASE_F(45, 2, 0x19, true);
    CERA_SPECIAL_CASE_F(46, 2, 0x1a, true);
    CERA_SPECIAL_CASE_F(47, 2, 0x1b, true);
    CERA_SPECIAL_CASE_F(742, 2, 0x25, true);
    CERA_SPECIAL_CASE_F(200, 2, 0x28, true);
    CERA_SPECIAL_CASE_F(2660013, 2, 0x2e, true);
    CERA_SPECIAL_CASE_F(2660051, 2, 0x30, true);
    CERA_SPECIAL_CASE_F(2675028, 2, 0x1f, true);
    CERA_SPECIAL_CASE_F(2670266, 2, 0x46, true);
    CERA_SPECIAL_CASE_F(194, 2, 0x1f, true);
    CERA_SPECIAL_CASE_F(196, 2, 0x20, true);
    CERA_SPECIAL_CASE_F(720, 2, 0x23, true);
    CERA_SPECIAL_CASE_F(197, 2, 0x21, true);
    CERA_SPECIAL_CASE(195, 9, 0);
    CERA_SPECIAL_CASE_F(198, 2, 0x21, true);
    CERA_SPECIAL_CASE_F(743, 2, 0x26, true);
    CERA_SPECIAL_CASE_F(744, 2, 0x27, true);
    CERA_SPECIAL_CASE_F(820, 2, 0x29, true);
    CERA_SPECIAL_CASE_F(920, 2, 0x2c, true);
    CERA_SPECIAL_CASE_F(2660290, 2, 0x21, true);
    CERA_SPECIAL_CASE_F(2660358, 2, 0x21, true);
    CERA_SPECIAL_CASE_F(2660401, 2, 0x23, true);
    CERA_SPECIAL_CASE_F(2660402, 2, 0x26, true);
    CERA_SPECIAL_CASE(821, 10, 0);
    CERA_SPECIAL_CASE_F(2660409, 2, 0x4c, true);
    CERA_SPECIAL_CASE_F(2660410, 2, 0x4d, true);
    CERA_SPECIAL_CASE_F(2660411, 2, 0x4e, true);
    CERA_SPECIAL_CASE_F(2660408, 2, 0x4f, true);
    CERA_SPECIAL_CASE_F(2660703, 2, 0x4f, true);
    CERA_SPECIAL_CASE_F(2660543, 2, 0x50, true);
    CERA_SPECIAL_CASE_F(2660544, 2, 0x53, true);
    // b28d-solo 真语义 bug 修复：ORIG 0x0832506e 段存在 outValue=0x54 的叶
    // （Ghidra 树以 `if (param_1 < 0x2898c2) { *param_3=0x54; return 1; }` 表达
    // 0x2898c1 这一未列 case）⇒ 源码补 case 2660545（0x2898C1）。
    CERA_SPECIAL_CASE_F(2660545, 2, 0x54, true);
    CERA_SPECIAL_CASE_F(2660546, 2, 0x57, true);
    CERA_SPECIAL_CASE_F(10000391, 2, 0x5d, true);
    CERA_SPECIAL_CASE_F(10000388, 2, 0x59, true);
    CERA_SPECIAL_CASE_F(10000389, 2, 0x5a, true);
    CERA_SPECIAL_CASE_F(10000390, 2, 0x5b, true);
    CERA_SPECIAL_CASE_F(10000387, 2, 0x5c, true);
    CERA_SPECIAL_CASE_F(690000221, 0xd, 10, true);
    CERA_SPECIAL_CASE_F(690000222, 0xd, 0x32, true);
    CERA_SPECIAL_CASE_F(690000223, 0xd, 100, true);
    CERA_SPECIAL_CASE_F(690000224, 0xd, 500, true);
    CERA_SPECIAL_CASE_F(690000225, 0xd, 1000, true);

    default:
        break;
    }

    {
    // 默认路径（ORIG +0xd0d）：可堆叠物品按 GetItemType 分类。
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item != 0 && item->is_stackable())
    {
        // ORIG +0xd5b..+0xe0c：GetItemType() 虚调用独立 5 次（无具名缓存，P0），
        // 0xf/0x10 双臂为 `(t==0xf)||(t==0x10)` 两次独立调用短路。
        if (item->GetItemType() == 0x26)
            return false;
        if (item->GetItemType() == 0xe)
        {
            outType = 5;
            flag = true;
            return true;
        }
        if (item->GetItemType() == 0xf || item->GetItemType() == 0x10)
        {
            outType = 6;
            flag = true;
            return true;
        }
        if (item->GetItemType() == 0x22)
        {
            outType = 0xc;
            return true;
        }
    }
    return false;
    }

L_true:
    return true;
}

#undef CERA_SPECIAL_CASE
#undef CERA_SPECIAL_CASE_F

// ============================================================================
// ProcessSpecialItem —— ORIG 0x0832587e (T)
// ============================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0832587e orig/ours=125/108 ndiff=108。
// 帧/槽号平移 + STL 内联差；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=NEAR [N93] 125/125 等长，map end/find 迭代器槽方向相反（end -0x18 vs -0x24）
// [b28d-poolcera P326 翻案命中] end 迭代器不具名、右起内联 `== m.end()` ⇒ 临时槽
// 阶梯 end -0x18 / find -0x14 与 ORIG 逐槽一致 ⇒ **IDENTICAL 125/125 ndiff=0**。
// 旧 N93「槽方向无源码解」作废（P326 三级阶梯形，本件实证并列）。
#include "CCeraShop_Special/ProcessSpecialItem.inc"
                                   int& restrictCode, long curSec, bool flag,
                                   std::vector<Inven_Item>& items,
                                   stCeraShopItemParam_t& param,
                                   char c1, char c2, bool flag2, int extra)
{
    if (user->getCurCharacR() == 0)
        return false;

    int type = 0;
    int value = 0;
    bool f = false;

    // ORIG 0x0832587e：先 end() 后 find(itemIdx) 再 operator==（P293 求值序），
    // 命中（迭代器不相等）时 type=2 直落 else 块，未命中才走 IsSpecialItem。
    // [b28d-poolcera P326 重试] end 不具名（具名槽落 -0x24 深），右操作数右起
    // 先求值 ⇒ end 临时 -0x18 / find 临时 -0x14，与 ORIG 逐槽一致。
    if (g_map_premium_item_data.find((unsigned long)itemIdx) ==
        g_map_premium_item_data.end())
    {
        if (!IsSpecialItem(itemIdx, type, value, f))
            return false;
    }
    else
    {
        type = 2;
    }

    if (count == 0 && (itemIdx == 0x10 || itemIdx == 0x28d171))
        count = 50000;

    // ORIG 死存储（-0xc 槽）：结果未被使用但 O0 保留读取。
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    (void)lethe;

    STSpecailItem_Param p(items, param);
    // ORIG 字段写入序：value → user → itemIdx → count → time → flag → c1/c2/flag2 → extra。
    p.m_value = value;
    p.m_user = user;
    p.m_itemIdx = itemIdx;
    p.m_count = count;
    p.m_time = curSec;
    p.m_flag = flag;
    p.m_char1d = (unsigned char)c1;
    p.m_char1e = (unsigned char)c2;
    p.m_char1f = flag2;
    p.m_extra = extra;

    // m_handler：CCeraShop +0x00 的 SpecialItemHandler*（权威具名成员，ctor new 填充）。
    STSpecailItem_Result result;
    if (!m_handler->execute(type, p, result))
        return false;

    restrictCode = result.m_value;
    return true;
}

// ============================================================================
// _CheckBuyingCondition —— ORIG 0x08324810 (T)
//   CCeraShopGoods 重载。groupIdx==1 且 param6==0：按金币家庭（GetCoin+GetPayCoin+
//   GetEventCoin）+stackCount 判定金币上限（<0x186a1 通过，否则 outCode=0x4f）。
//   其余 groupIdx 委托 uint 重载 _CheckBuyingCondition(CUser*, uint, ulong, uint,
//   ushort, uchar, ushort)；子返回非 1 则 outCode=8。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08324810 orig/ours=102/97 ndiff=92。
// 帧/槽号与寄存器分配差（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P0 内联三实参 + P292 uint 重载 bool 化 + 块序翻转
#include "CCeraShop_Special/_CheckBuyingCondition_1.inc"
                                      unsigned int param3, unsigned char param4,
                                      unsigned short param5, char param6, int& outCode)
{
    if (goods->GetGroupIdx() == 1 && param6 == 0)
    {
        // ORIG：三取值各自现场 getCurCharacInvenR（P48/P183 禁缓存）。
        unsigned int coin = user->getCurCharacInvenR()->GetCoin() +
                            user->getCurCharacInvenR()->GetPayCoin() +
                            user->getCurCharacInvenR()->GetEventCoin();
        unsigned int stackCount = (unsigned int)goods->GetStackCount();
        if ((int)(coin + stackCount) > 0x186a0)
        {
            outCode = 0x4f;
            return false;
        }
        outCode = 0;
        return true;
    }
    // P0：ORIG 委托段三实参无槽往返（esi/ebx/eax 直传）⇒ 内联实参。
    bool ok = false;
    ok = _CheckBuyingCondition(user, param3, (unsigned long)goods->GetItemIdx(),
                               (unsigned int)goods->GetStackCount(),
                               (unsigned short)goods->GetExpiryDate(), param4,
                               param5);
    if (!ok)
        outCode = 8;
    return ok;
}

// ============================================================================
// _CheckValidCount —— ORIG 0x08323af4 (T)
//   双重校验：1) 条目 count 累计（IsSpecialItem 分组），任一分组 >1 则失败；
//   2) 对每个 stack-buyable 商品，按 restrictCode 校验限制购买仍为可重复。
//   返回 true=通过；其间任何限制冲突返回 false。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08323af4 orig/ours=320/327 ndiff=311。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 324/331；P214③ rep stos 与 P0
// type/value 循环外前置清零（ORIG -0x44/-0x48）已复现，P167 CLEAR 33/33；
// 余帧 0xbc/0xcc（16k 量化不可凑）+ 4.1/4.4 槽复用序差。
#include "CCeraShop_Special/_CheckValidCount.inc"
{
    // P214③：ORIG 0x08323af4 用 `mov $0,%eax; mov $0xf,%edx; rep stos`（0x3c 字节）
    // ⇒ 显式零初始化形（memset 会退化为 call）。
    int typeCount[15] = {0};

    // ORIG 0x08323b16/0x08323b1d：type/value 在**循环外**一次性清零
    // （`movl $0,-0x44` / `movl $0,-0x48`），循环内仅 specialFlag 逐轮清零
    // （0x08323c1b `movb $0,-0x55`）⇒ 两出参声明在循环外。
    int type = 0;
    int value = 0;
    int n = 0;
    // ORIG 0x08323cd3/0x08323c09：循环条件仅 `n <= 0x22`（setle 有符号），
    // `goodsList[n]==0` 是体首独立 break（0x08323b3d/0x08323b5e 各落 nop 岛）。
    for (; n <= 0x22; ++n)
    {
        if (goodsList[n] == 0)
            break;
        for (int j = n + 1; j <= 0x22; ++j)
        {
            if (goodsList[j] == 0)
                break;
            if (goodsList[n]->GetItemIdx() == goodsList[j]->GetItemIdx())
            {
                // ORIG 0x08323ba8：GetGroupIdx() 结果留 %eax 连比 3/7/0（无槽）
                // ⇒ switch（P251：无槽多路相等判断原码是 switch）。
                switch (goodsList[n]->GetGroupIdx())
                {
                case 3:
                {
                    // 实测：帧 0xcc→0xbc（=ORIG）；报告 diff 207/205→200/199；
                    // our 326→325（ORIG 324）；TU 四数不变 ⇒ 保留。
                    // [G10 实验·P231/P52] ORIG 浅区槽比我方少 4（我方多 0x38/0x5c/
                    // 0x60/0x64）；item 跨 is_stackable 虚调保活 ⇒ 试纯 register（P0 合规）。
                    register CItem* item = G_CDataManager()->find_item(goodsList[n]->GetItemIdx());
                    // ORIG 0x08323be9 无判空（P140/P171：不补边界防御）。
                    if (!item->is_stackable())
                        return false;
                    break;
                }
                case 7:
                case 0:
                    return false;
                }
            }
        }

        bool specialFlag = false;
        // ORIG 0x08323c32：this 用全局 `GlobalData::s_pCeraShop`（内存装载），
        // 非隐式 this（0x8(%ebp)）；`xor $1; test; jne` ⇒ 否定早退形。
        if (!GlobalData::s_pCeraShop->IsSpecialItem(
                goodsList[n]->GetItemIdx(), type, value, specialFlag))
            continue;
        {
            ++typeCount[type];
            if (specialFlag)
            {
                // ORIG 0x8323cf4：以 std::max/std::min 实调钳制
                // min(1, max(count, count-1))（special 商品出现即回 1，
                // 不参与限购超额判定；两算法调用点照抄）。
                int dec = typeCount[type] - 1;
                int mx = std::max(typeCount[type], dec);
                int one = 1;
                typeCount[type] = std::min(one, mx);
            }
        }
    }

    for (int i = 1; i < 0xf; ++i)
    {
        if (typeCount[i] > 1)
            return false;
    }

    // 可叠购买商品按 restrictCode 校验（CItemList::GetRestrictCode）。
    typedef std::pair<const CCeraShopGoods*, unsigned int> GoodsCount;
    std::vector<GoodsCount> counts;
    for (int i = 0; i < 0x23; ++i)
    {
        if (goodsList[i] == 0)
            continue;
        if (goodsList[i]->isStackBuyable())
        {
            int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(
                goodsList[i]->GetItemIdx());
            if (restrictCode != 0)
            {
                // 按商品指针找到已有计数并自增；ORIG 以循环后 it==end() 判缺
                // （operator== 调用点，无 bool 标志；begin/end 各一次、判缺复用
                // 迭代器无 end 重发）。
                std::vector<GoodsCount>::iterator itBeg = counts.begin();
                std::vector<GoodsCount>::iterator itEnd = counts.end();
                for (; itBeg != itEnd; ++itBeg)
                {
                    if (itBeg->first == goodsList[i])
                    {
                        ++(itBeg->second);
                        break;
                    }
                }
                if (itBeg == itEnd)
                    counts.push_back(GoodsCount(goodsList[i], 1));
            }
        }
    }

    for (std::vector<GoodsCount>::iterator it = counts.begin();
         it != counts.end(); ++it)
    {
        if (it->second > 1)
            return false;
    }

    return true;
}

// ============================================================================
// ProcessError —— ORIG 0x08323978 (T)
// 语义：构造 0x43 错误包（header 1/0x43），按调用方错误码序列化，Send 后记
// cMyTrace "NEXON BILLING : Buy Error(%d,%u)"。
// C 反编译：put_header(1,0x43); put_byte(0); put_byte(0); put_short(param_2);
//           put_byte(param_7); put_int(param_3); put_int(param_4);
//           put_int(param_5); put_int(param_6); put_int(-1); finalize(true);
//           user->Send(packet); cMyTrace(...)。
// ============================================================================
#include "CCeraShop_Special/ProcessError.inc"
                             int d, int e, char f)
{
    PacketGuard packet;
    packet.put_header(1, 0x43);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_short(a);
    packet.put_byte(f);
    packet.put_int(b);
    packet.put_int(c);
    packet.put_int(d);
    packet.put_int(e);
    packet.put_int(-1);
    packet.finalize(true);
    user->Send(packet);

    // b28d-D42 P227/P221：ORIG 0x08323978 中 trace 匿名槽(-0x18)浅于
    // PacketGuard(-0x24)，且 (int)a 在 ctor 前装载 %ebx 跨 ctor 保活
    // （`movzwl -0x2c(%ebp),%ebx` 先于 cMyTrace C1）⇒ 全表达式临时链形。
    cMyTrace(__PRETTY_FUNCTION__, 0xc2a, 0)(
        "NEXON BILLING : Buy Error(%d,%u)", (int)a, c);
}

// ============================================================================
// SaveFeaturedIdx —— ORIG 0x08325a30 (T)
// ============================================================================
#include "CCeraShop_Special/SaveFeaturedIdx.inc"
{
    if (restrictCode == 0)
        return;
    // ORIG 0x8325a50：characNo 跨 call 存 %ebx、accId eax 直传出参槽（P24/P52
    // 单表达式实参形，无具名槽）。
    DB_SaveFeaturedGoodsIdx::makeRequest(user->get_acc_id(), user->get_charac_no(-1),
                                         restrictCode, curSec, d);
}

// ============================================================================
// setOriginalItem —— ORIG 0x08327ee6 (T)：记录原始商品 idx 到 m_originalItemIdx。
// ============================================================================
#include "CCeraShop_Special/setOriginalItem.inc"
{
    m_originalItemIdx = itemIdx;
}

// ============================================================================
// getOriginalItem —— ORIG 0x08327ef4 (T)：读取记录的原始商品 idx（<=0 返回 0）。
// ============================================================================
#include "CCeraShop_Special/getOriginalItem.inc"
{
    // ORIG 0x08327ef4：if-return 分支岛形（4.4 三目会折 cmovs，异形）。
    if (m_originalItemIdx > 0)
        return m_originalItemIdx;
    return 0;
}

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

// ============================================================================
// _processCoin —— ORIG 0x08325a88 (T)
// 代币购买入账：PayCoin += value → historyLog.PayCoinAdd(总额, 增量, reason 3)
// → SaveMoneyCoin → SendCoin。第 3 参 flag 在 ORIG 中未参与逻辑（签名保留）。
// ============================================================================
#include "CCeraShop_Special/_processCoin.inc"
{
    user->getCurCharacInvenW()->SetPayCoin(
        user->getCurCharacInvenR()->GetPayCoin() + value);
    user->m_historyLog.PayCoinAdd(
        (int)user->getCurCharacInvenR()->GetPayCoin(), (int)value,
        (eCoinAddReason)3);
    user->SaveMoneyCoin();
    user->SendCoin();
}

// ============================================================================
// autoUnsealItem —— ORIG 0x08327f10 (T)
// 自动解封：商品为随机选项物品（IsRandomOption）且原商品
// （getOriginalItem→find_item）IsRandomOptionUnseal 时，经
// GlobalData::g_randomOptionHandle->give_option 写入 invItem 的随机选项区
// （m_random.m_random，ORIG &invItem+0x25）。失败记 logFormat。
// ============================================================================
// b28d-D42 verdict=DIFF [N18a] ORIG 0x08327f10 orig/ours=103/105 ndiff=78。
// xor/sete 物化方向与寄存器分配差；语义等价封顶。
// b28d-solo-CCeraShop_Special verdict=NEAR [N7] 105/105 等长，仅 give_option 实参 edx/ecx 角色互换
#include "CCeraShop_Special/autoUnsealItem.inc"
{
    // b28d-solo P223 多岛形：ORIG 两守卫各落独立 nop 岛（item==0 → nop;jmp；
    // !IsRandomOption → nop），合并 && 会被 4.4 归一成 bool 岛 ⇒ 拆两条独立早返。
    if (item == 0)
        return;
    if (!item->IsRandomOption())
        return;
    int original = getOriginalItem();
    if (original != 0)
    {
        const CItem* origItem = G_CDataManager()->find_item(original);
        if (origItem != 0 && origItem->IsRandomOptionUnseal())
        {
            // ORIG 实参序 vtable 槽核对 —— get_rarity/getUsableLevel/
            // getItemGroupName/GetRandomOptionGrade(vtable+0x50)；
            // options = invItem.m_random.m_random（ORIG &invItem+0x25）。
            if (!GlobalData::g_randomOptionHandle->give_option(
                    invItem.m_addInfo,
                    (ENUM_RARITY)item->get_rarity(), item->getUsableLevel(),
                    item->getItemGroupName(), item->GetRandomOptionGrade(),
                    &invItem.m_random.m_random))
            {
                LogManager::logFormat(1, "CeraShop.cpp", __PRETTY_FUNCTION__,
                                      0x2131, "autoUnsealItem : %d",
                                      invItem.m_addInfo);
            }
        }
    }
}

// ============================================================================
// _processItem —— ORIG 0x08325b22 (T)
// 一般可叠/装备商品主路径：ProcessSpecialItem 扣费分发（c1=c2=0、flag2=false、
// extra=item.m_addInfo2）→ AvatarCoin::BuyProcess → insertItemIntoInventory(0xf)
// → SendUpdateItemList（artifact 走 ITEMSPACE 7）→ save 时 SaveInventory；
// 入包失败走 ReqDBSendNewMailCashShop 邮件兜底并返回 0。
// ============================================================================
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P169 外层正向 if + goto 尾标签（return slot 直落 epilogue）
#include "CCeraShop_Special/_processItem.inc"
                            long curSec, bool flag, bool save,
                            std::vector<Inven_Item>& items,
                            stCeraShopItemParam_t& param)
{
    // b28d-D42 verdict=DIFF [N93附] ORIG 0x08325b22 orig/ours=147/146 ndiff=8。
    // 残差仅尾部双 return 块发射序相反（ORIG ret0 块在 ret-slot 块之前；
    // N93附 四形探针全败）；语义等价封顶。
    int slot = -1;
    if (!ProcessSpecialItem(user, item.m_addInfo, count, restrictCode, curSec,
                            flag, items, param, 0, 0, false, item.m_addInfo2))
    {
        if (AvatarCoin::BuyProcess(user, item))
            return 0;

        if ((slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                 item, (eItemAddReason)0xf, true, false)) != -1)
        {
            if (item.m_field1 == Inven_Item::ITEM_TYPE_ARTIFACT)
                user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7,
                                         slot);
            else
                user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                         slot);
            if (save)
                user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104",
                                                  (bool*)0);
            WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, item, 0, (unsigned int)user->getCurCharacNo(), true, msg,
                (int)std::strlen(msg));
            slot = 0;
        }
        goto end;
    }
    return 0;
end:
    return slot;
}

// ============================================================================
// _processCreature —— ORIG 0x08325d66 (T)
// 造物（creature）商品：CCreatureMgr::CheckItem 预检（失败置 outFlag=1）→
// type==5（蛋）时 SetEggItem 孵化转换（失败记 History ERROR 并置 outFlag=1）→
// insertItemIntoInventory(0x19)：成功则 InsertCreatureItem + 刷新列表 +
// SaveInventory，outCode=GetInvenSlot(3,slot).m_addInfo；失败按类型走
// Creature/CashShop 邮件兜底，outCode=item.m_addInfo、返回 0。
// ============================================================================
#include "CCeraShop_Special/_processCreature.inc"
                                int& outFlag, bool, int usablePeriod,
                                int expirationDate)
{
    int slot = -1;
    bool checkOk = true;
    if (!user->getCurCharacInvenW()->GetCreatureMgrW()->CheckItem(user, &item,
                                                                  checkOk))
        outFlag = 1;
    if (checkOk)
    {
        if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
            {
                LogManager::logFormat(
                    1, "CeraShop.cpp",
                    __PRETTY_FUNCTION__,
                    0x171f, "History ERROR, CCreatureItemConverter::SetEggItem");
                outFlag = 1;
            }
        }

        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)0x19, true, false);
        if (slot < 0)
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104",
                                                  (bool*)0);
            if (item.m_field1 == Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
                WongWork::CMailBoxHelper::ReqDBSendNewCreatureMail(
                    user, user->getCurCharacNo(), item, msg,
                    (int)std::strlen(msg));
            else
                WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                    user, item, 0, (unsigned int)user->getCurCharacNo(), true,
                    msg, (int)std::strlen(msg));
            outCode = item.m_addInfo;
            return 0;
        }
        else
        {
            user->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
                &item, slot, 0x19, usablePeriod, expirationDate);
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)7,
                                     slot);
            user->SaveInventory();
            Inven_Item placed = user->getCurCharacInvenR()->GetInvenSlot(3, slot);
            outCode = placed.m_addInfo;  // ORIG 读 sret 缓冲 +2 处 dword
        }
    }
    return slot;
}

// ============================================================================
// _sendCargoUpgradePacket —— ORIG 0x08327726 (T)
// 组 0x42 仓库扩容包：header(0,0x42) / short(type) / short(GetCapacity)。
// ============================================================================
#include "CCeraShop_Special/_sendCargoUpgradePacket.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    packet.put_short(user->getCurCharacCargoR()->GetCapacity());  // ORIG 无具名槽（P0 右值内联直传）
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _sendCharacSlotExtendPacket —— ORIG 0x08327674 (T)
// 组 0x42 角色槽扩展包：header(0,0x42) / short(type) / byte(getCharacSlotLimit()).
// ============================================================================
#include "CCeraShop_Special/_sendCharacSlotExtendPacket.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    packet.put_byte(user->getCharacSlotLimit() & 0xff);  // ORIG 无具名槽（P0）
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _sendInventoryUpgradePacket —— ORIG 0x083275be (T)
// 组 0x42 背包扩容包：header(0,0x42) / short(type) / short(get_inventory_capacity()).
// ============================================================================
#include "CCeraShop_Special/_sendInventoryUpgradePacket.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(type);
    packet.put_short(user->getCurCharacInvenR()->get_inventory_capacity());  // ORIG 无具名槽（P0）
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _sendPremiumPacket —— ORIG 0x083277dc (T)
// 组 0x42 高级会员购买通知包：header(0,0x42)/short(param2)/byte(param3)/int(param4)。
// ============================================================================
#include "CCeraShop_Special/_sendPremiumPacket.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(a);
    packet.put_byte(type);          // ORIG 全宽装载（mov 0x14(%ebp)），禁 uchar cast（生 movzbl）
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// _ClosePremium —— ORIG 0x08327bb6 (T)
// 关闭 type 指定 premium：取 GetPremiumInfo(type)，非空则逐项投递 LOGDB_Q
// （0x54 头 + uid + accId + type + curSec + curSec+1 + flag + serverCode），
// 并调 CHandlePremium::handleSetUserPremium / handlePremiumEffect /
// handleNotifyPremiumInfo。
// ============================================================================
#include "CCeraShop_Special/_ClosePremium.inc"
{
    WongWork::CUserPremium* premium = user->GetPremiumInfo();
    std::vector<WongWork::SUserPremiumInfo> list;
    // ORIG 0x08327be4/0x8327bfa：GetPremiumInfo(type) 判空与取值两次独立调用
    // （P48 无 CSE）。
    if (premium->GetPremiumInfo(type)->m_type != 0)
        list.push_back(*premium->GetPremiumInfo(type));

    int curSec = GlobalData::s_systemTime_.getCurSec();
    int nextSec = curSec + 1;
    unsigned int serverCode = 0;
    // ORIG 0x8327c4e..：非 const iterator for（begin 一次、end 逐轮、7×
    // operator-> 逐点重发，P157/P175 —— 禁缓存引用/索引）。
    for (std::vector<WongWork::SUserPremiumInfo>::iterator it = list.begin();
         it != list.end(); ++it)
    {
        serverCode = 0;
        // m_useTargetServer(+0x5c)==1 → serverCode = m_serverEnvir.m_serverGroup
        // （CEnvironment+0x378）
        if (g_SPremiumInfo[it->m_type].m_useTargetServer == 1)
        {
            serverCode = (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        }
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x20e7),
                               true);
        guard.operator*()->operator<<((int)0x54);   // 0x54 头（ORIG lsEi）
        guard.operator*()->operator<<((int)user->GetUID());   // lsEi
        guard.operator*()->operator<<((unsigned int)user->get_acc_id());  // ORIG lsEj
        guard.operator*()->operator<<((int)it->m_type);   // lsEi
        guard.operator*()->operator<<((long)curSec);    // ORIG lsEl
        guard.operator*()->operator<<((long)nextSec);   // ORIG lsEl
        guard.operator*()->operator<<((int)it->m_flag);   // lsEi
        guard.operator*()->operator<<((unsigned int)serverCode);  // ORIG lsEj
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        WongWork::CHandlePremium::handleSetUserPremium(
            user, (ENUM_PREMIUM_TYPE)it->m_type, curSec, nextSec, it->m_flag, false);
        WongWork::CHandlePremium::handlePremiumEffect(user,
            (ENUM_PREMIUM_TYPE)it->m_type);
        WongWork::CHandlePremium::handleNotifyPremiumInfo(user, it->m_type, 0);
    }
}

// ============================================================================
// _onBuySpecialItem —— ORIG 0x08327454 (T)
// itemIdx ∈ 特殊集（0x3f..0x49 除 0x44，掩码 0x7df）时，投递
// SIG_SAVE_CRAZY_LEVELUP_EVENT 流（"0707" 头 + accId + characNo(-1)）至 LOGDB_Q；
// 再按 Is2010ChuseokPacket(itemIdx) 分支调 DB_UpdateChuseokPackagePurchase。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08327454 orig/ours=97/102 ndiff=79。
// 槽/寄存器分配差（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N6/N99] 97/99；(1u<<k)&0x7df 被 4.4 恒折
// (0x7df>>k)&1 + k 重算。探针 14 形（内联/具名/register/volatile/双 register/bit 拆分）
// 全产 shr，shl 形不可择 ⇒ N6/N99 封顶。P167 CLEAR 18/18。
#include "CCeraShop_Special/_onBuySpecialItem.inc"
                                  unsigned long itemIdx)
{
    // ORIG: (param_4 - 0x3f < 0xb) && ((1 << ((param_4 - 0x3f) & 0x1f)) & 0x7df) != 0
    if ((unsigned int)(itemIdx - 0x3f) < 0xb &&
        ((1u << (itemIdx - 0x3f)) & 0x7dfu) != 0)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x1971),
                               true);
        guard.operator*()->operator<<((int)0xb4);
        guard.operator*()->operator<<(-1);
        SaveCrazyLevelupPayloadView* buf =
            reinterpret_cast<SaveCrazyLevelupPayloadView*>(
                guard->GetInBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
        std::memcpy(buf->m_tag, "0707", 5);  // DAT_08c25e65 = "0707\0"
        buf->m_accId = user->get_acc_id();
        buf->m_characNo = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    if (Is2010ChuseokPacket(itemIdx))
        DB_UpdateChuseokPackagePurchase::makeRequest(user->get_acc_id());
}

// ============================================================================
// _processBoosterSelect —— ORIG 0x08327894 (T)
// 在 booster 表（m_booster.m_map，item+0x24c，key=pair<char,char>(c1,c2)）中定位，
// 收集其 value 中与 items 现有 Inven_Item（m_addInfo==m_itemIdx）匹配的子物品，
// 清空 items 后逐项 _processAdditionalItem；随后限制商品校验 + SaveInventory。
// 返回：find==end→0；item==0 或处理完成→1。
// ============================================================================
// b28d-D42 verdict=DIFF [N82] ORIG 0x08327894 orig/ours=233/238 ndiff=215。
// 帧/槽号平移（P112/N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 233/229 ndiff 85→68；P183/P157 删
// stackable/bmap 具名缓存（ORIG 0x08327894 每处 `mov item,%eax; lea 0x24c` 重算）+
// P323 `static_cast<const M&>` 视图 cast 消 iterator→const_iterator 转换构造；
// P167 CLEAR 34/34；余帧/槽平移封 N82。
// b28d-sweepCCeraShop（本轮逐位复核）233/229：jcc 目标下标逐条对齐（je/je/seta/jne×3/je/je
// 语义全同）、p167 CLEAR、无 `call *`；gap 4 = ORIG 的 esi/ebx 双寄存器共享尾
// （`je +0x310` 直达 `mov $1,%esi` + 尾部 `test %ebx,%ebx` 物化）vs 4.4 单 ebx 直返
// （`jne; mov $1,%ebx; jmp` + `mov %ebx,%eax`）共 ±4，加 +0xd/+0xe 寄存器 vs 槽分配差。
#include "CCeraShop_Special/_processBoosterSelect.inc"
                                     long curSec, bool flag,
                                     std::vector<Inven_Item>& items,
                                     stCeraShopItemParam_t& param, char c1, char c2)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return 1;

    std::vector<stStackableBoosterItemInfo_t> boosterList;
    // ORIG const 视图（find/end/==/-> 与 vector operator[] 全部 const 形；
    // P167：非 const 形会多发 2 个 const_iterator 转换构造并选错 find/end 重载）。
    // ORIG 0x08327894：命中（it != end）走主流程并置 ok=true，else 置 ok=false，
    // 尾部统一 `if(!ok) return 0; return 1;`（局部 vector 析构在 return 之前）。
    // P183/P157：ORIG 0x08327894 无 stackable/bmap 具名槽——map 基址每处
    // 现场重算 `mov item,%eax; lea 0x24c(%eax),%ecx`（禁指针/引用缓存）。
    // const 视图：ORIG 调 `_ZNKSt3map…4find`/`end`（const 形，无 iterator→
    // const_iterator 转换构造），故按 const CStackableItem* 内联访问。
    // 4.4 对 `static_cast<const C*>(p)->m.method()` 内联形仍选非 const 重载
    // （探针 p2.cpp j vs i）；显式 `static_cast<const M&>` 视图 cast 才出
    // `_ZNKSt3map…find` 且不占额外槽。
    typedef std::map<std::pair<char, char>,
                     std::vector<stStackableBoosterItemInfo_t> > BoosterMap_t;
    BoosterMap_t::const_iterator it =
        static_cast<const BoosterMap_t&>(
            static_cast<const CStackableItem*>(item)->m_booster.m_map).find(
            std::make_pair(c1, c2));
    // ORIG 0x0832794d/0x08327b73/0x08327ba0 的 esi/ebx 共享尾出口为 4.1 对
    // `return 0;`/`return 1;` 的布局产物（4.4 把 return 1 直接落 ebx）⇒ N 封顶。
    if (it == static_cast<const BoosterMap_t&>(
                  static_cast<const CStackableItem*>(item)->m_booster.m_map).end())
        return 0;
    {
        const std::vector<stStackableBoosterItemInfo_t>& src = it->second;
        // ORIG 0x083279fa `setg %al`：外层索引 i 为 int（signed 比较）；内层
        // 0x083279e4 `seta %al` 为 unsigned size_type。
        for (int i = 0; i < (int)items.size(); ++i)
        {
            for (std::vector<stStackableBoosterItemInfo_t>::size_type j = 0;
                 j < src.size(); ++j)
            {
                if (src[j].m_itemIdx == items[i].m_addInfo)
                    boosterList.push_back(src[j]);
            }
        }

        items.clear();

        for (std::vector<stStackableBoosterItemInfo_t>::iterator b =
                 boosterList.begin();
             b != boosterList.end(); ++b)
        {
            // ORIG 实参顺序：itemIdx(+0), addInfo(+0x8), avatarLevel(+0xe),
            // avatarType(+0xd), items, param, +0xa(flag)
            _processAdditionalItem(user, (unsigned long)b->m_itemIdx, b->m_addInfo,
                                   b->m_avatarLevel, b->m_avatarType, items, param,
                                   b->m_byteA);
        }

        int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
        // b28d-solo(P225 推广)：`&&` 被 4.4 折成 bool 物化岛（mov $1/jmp/mov $0/
        // test/je，+4 条）；ORIG 两条件各发独立 je 直跳 ⇒ 拆嵌套 if 逐条复现。
        if (restrictCode != 0)
        {
            if (!user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
            }
        }
        user->SaveInventory();
    }
    return 1;
}

// ============================================================================
// _processAdditionalItem —— ORIG 0x083267a2 (T)
// 结构（报告 /tmp/pai_report.md 全量逐块对照）：
//   getCurCharacR()==0 → return；itemIdx==1 → _processCoin + Inven_Item + push_back。
//   否则 errIdx 局部恒 -1 → ORIG -O0 下 `if (errIdx != -1)` 为死块（P184 照抄：
//   两枚 PacketGuard 发包 + Inven_Item 推入），else 主路径：
//   find_item；可叠特殊物品（type 0x10/0x22）→ insertItemIntoInventory/邮件兜底+push_back；
//   一般路径：agency 清零 + genIPGNo(头像) + Inven_Item 组装 + SetUpgrade(flag/五值) +
//   autoUnsealItem + 计数循环（头像 AddAvatarItem/邮件；造物 _processCreature；
//   否则 _processItem（0xc1 时前置双 find + 嵌套发放 0x10 count=0xc352））。
// 【§6.2 验收标注】orig=765 our=732；P167 调用点差集归零（仅余 N78 两处
// rep stos→memset call：nameBuf 0x200 / agency 0x18）；其余 |gap| 为 N82 帧
// 槽号 + P184 死块 rep 段形态残余 ⇒ 语义完成、byte 层 N 封顶。
// ============================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x083267a2 orig/ours=763/730 ndiff=702。
// 巨件帧槽（N82）+ N78 rep stos→memset call 两处（nameBuf 0x200/agency 0x18）；
// P167 调用点差集已归零。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 763/765；P167 CLEAR 75/75（memset→{0} 消 N78 两处），余帧 0x47c/0x46c
#include "CCeraShop_Special/_processAdditionalItem.inc"
                                       unsigned short slot, unsigned char a,
                                       unsigned char b, std::vector<Inven_Item>& items,
                                       stCeraShopItemParam_t& param, unsigned char flag)
{
    if (user->getCurCharacR() == 0)
        return;

    if (itemIdx == 1)
    {
        _processCoin(user, slot, false);
        Inven_Item coin;
        coin.m_addInfo = 1;
        coin.m_addInfo2 = (int)slot;
        items.push_back(coin);
        return;
    }

    int notifySize = 0;      // ORIG -0x5c
    int notifyCount = 0;     // ORIG -0x58
    int errIdx = -1;         // ORIG -0x54；-O0 下恒 -1 → 下方 if 为死块（P184）
    // P214③：ORIG 对两处缓冲用 rep stos 内联清零（N78 突破口）⇒ 显式 {0} 零初始化。
    char nameBuf[0x200] = {0};     // ORIG -0x3ed
    int nameLen = 0;         // ORIG -0x50

    if (errIdx != -1)
    {
        // —— -O0 静态不可达块：照抄 ORIG 0x134..0x303 发射序 ——
        PacketGuard notify;
        notify.put_header(1, notifySize);
        notify.put_byte(1);
        notify.put_int(notifyCount);
        notify.finalize(true);
        user->Send(notify);

        Inven_Item errItem;
        errItem.m_addInfo = errIdx;
        errItem.m_addInfo2 = (int)slot;
        items.push_back(errItem);

        PacketGuard errorMsg;
        errorMsg.put_header(0, 0xc);
        errorMsg.put_byte(0);
        errorMsg.put_short(0);
        errorMsg.put_byte(0);
        errorMsg.put_int(nameLen);
        errorMsg.put_str(nameBuf, nameLen);
        errorMsg.finalize(true);
        user->Send(errorMsg);
    }
    else
    {
        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return;

        // 可叠特殊物品：is_stackable && GetItemType() ∈ {0x10, 0x22}
        // （ORIG 短路形：非 stackable 直落 false 岛；两次 GetItemType 独立求值 P48）
        if (item->is_stackable() &&
            (item->GetItemType() == 0x10 || item->GetItemType() == 0x22))
        {
            int resSlot = -1;                // ORIG 先写 -0x44=-1 再组装
            Inven_Item invItem;
            invItem.m_addInfo = (int)itemIdx;
            invItem.m_addInfo2 = (int)slot;
            item->make_item(invItem);
            resSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
                invItem, (eItemAddReason)0xf, true, false);
            if (resSlot != -1)
                user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, resSlot);
            else
                CMailBoxHelper::ReqDBSendNewMailCashShop(
                    user, invItem, 0, user->getCurCharacNo(), true, 0, 0);
            items.push_back(invItem);
            return;
        }

        // 一般路径
        unsigned int count = slot;
        if (item->is_stackable())
            count = 1;

        char agency[0x18] = {0};
        if (item->IsAvatarItem())
        {
            unsigned int characNo = (unsigned int)user->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)3, characNo, agency);
        }

        Inven_Item invItem;
        invItem.m_addInfo = (int)itemIdx;
        invItem.m_addInfo2 = (int)slot;
        item->make_item(invItem);

        if (flag != 0)
            invItem.SetUpgrade(flag);
        if (invItem.m_addInfo == 0x71db || invItem.m_addInfo == 0x72d0 ||
            invItem.m_addInfo == 0x742d || invItem.m_addInfo == 0x7556 ||
            invItem.m_addInfo == 0x7661)
            invItem.SetUpgrade(0xc);

        autoUnsealItem(item, invItem);

        int accCera = 0;  // ORIG -0x78 累积出参（_processItem/_processCreature 共用）

        for (int i = 0; i < (int)count; ++i)
        {
            if (!item->IsAvatarItem())
            {
                if (!item->IsCreatureItem())
                {
                    long curSec = GlobalData::s_systemTime_.getCurSec();
                    _processItem(user, invItem, 0, accCera, curSec, true, false,
                                 items, param);
                    // ORIG 双 find_item 先于条件（0xa5e..0xa76），0xc1 判定在后。
                    CItem* found = G_CDataManager()->find_item(invItem.m_addInfo);
                    CItem* item10 = G_CDataManager()->find_item(0x10);  // ORIG 无空判
                    if (invItem.m_addInfo == 0xc1 && found != 0)
                    {
                        std::vector<Inven_Item> nestedItems;
                        stCeraShopItemParam_t nestedParam;
                        int nestedRestrict = 0;
                        Inven_Item nestedItem;
                        item10->make_item(nestedItem);
                        nestedItem.m_addInfo = 0x10;
                        nestedItem.m_addInfo2 = 1;
                        _processItem(user, nestedItem, 0xc352, nestedRestrict, curSec,
                                     false, true, nestedItems, nestedParam);
                    }
                }
                else
                {
                    int creatureSlot = 0;  // ORIG -0x94 出参
                    _processCreature(user, invItem, creatureSlot, accCera, false,
                                     item->getUsablePeriod(),
                                     item->getExpirationDate());
                }
            }
            else
            {
                int avatarPeriod = ((CEquipItem*)item)->getAvatarPeriod(b);
                int avSlot = user->getCurCharacInvenW()->AddAvatarItem(
                    (int)itemIdx, avatarPeriod, false, (char)a, (char)b, agency,
                    eAvatarItemAddReason_0, false, 0);
                if (avSlot >= 0)
                    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1,
                                             avSlot);
                else
                {
                    int expireDate = 0;
                    if (avatarPeriod != 0 && avatarPeriod != 0x16d)
                    {
                        // ORIG 0x08326f13..0x08326f2a：单表达式（call getCurSec →
                        // `mov -0x38,%edx; imul $0x15180,%edx,%edx; add %edx,%eax` →
                        // 存槽）；「赋值再 +=」会多发 mov+内存 add。
                        expireDate = GlobalData::s_systemTime_.getCurSec() +
                                     avatarPeriod * 0x15180;
                    }
                    const char* msg = g_scriptStringManager_.findString(
                        4, "game_server_msg_104", (bool*)0);
                    int msgLen = std::strlen(msg);
                    int characNo = user->getCurCharacNo();
                    CMailBoxHelper::ReqDBSendNewAvatarMail(
                        user, characNo, (int)itemIdx, expireDate, (char)a, (char)b,
                        agency, msg, msgLen);
                }
                user->SendAradEventItemLog(itemIdx, 1);
            }
            if (!item->is_stackable())
            {
                items.push_back(invItem);
            }
            else
            {
                if (!_isPackageItem(item))
                    items.push_back(invItem);
            }
        }
    }
}
}  // namespace WongWork

// ---- DB_SaveFeaturedGoodsIdx::makeRequest —— ORIG 0x0841b2da (T) ----
// 报告 docs/class_func_reports/DB_SaveFeaturedGoodsIdx/makeRequest.md 全文已读：
// <<0x52<<-1<<param_1(accId)<<param_2(characNo)<<param_3(restrictCode)
//   <<param_5(d)<<param_4(curSec) → put(DB_Q(2))。
#include "CCeraShop_Special/makeRequest.inc"
                                          int restrictCode, int curSec, int d)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4dfb),
                           true);
    guard.operator*()->operator<<((int)0x52);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)accId);
    guard.operator*()->operator<<((int)characNo);
    guard.operator*()->operator<<((int)restrictCode);
    guard.operator*()->operator<<((int)d);
    guard.operator*()->operator<<((int)curSec);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
