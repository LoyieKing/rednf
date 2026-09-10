// ============================================================================
// CUserDepsB.cpp —— CUser 第二组恢复（独占 TU；不触碰 CUser.cpp）。
//
// 八个方法均逐指令对照 docs/class_func_reports/CUser/<方法>.md 的
// ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   1. change_equip_expand          ORIG T 0x0865f382 _ZN5CUser19change_equip_expandEiiiiPi
//   2. eventDeliveryBuyItem         ORIG T 0x0866463c _ZN5CUser20eventDeliveryBuyItemEiRK10Inven_Item
//   3. _processLevelReward          ORIG T 0x0868745e _ZN5CUser19_processLevelRewardEjjc
//   4. _postQuestTriggerItem        ORIG T 0x086689ee _ZN5CUser21_postQuestTriggerItemEPK5Quest
//   5. processLevelUpEventReward    ORIG T 0x08663cc0 _ZN5CUser25processLevelUpEventRewardEb
//   6. processLevelUpEvent          ORIG T 0x0869115a _ZN5CUser19processLevelUpEventEv
//   7. getCurCharacInvenCheckSum    ORIG T 0x0864e0b6 _ZNK5CUser25getCurCharacInvenCheckSumEv
//   8. getCurCharacSkillCheckSum    ORIG T 0x0864e114 _ZNK5CUser25getCurCharacSkillCheckSumEv
//
// 另提供上述函数直接依赖且当前无任何定义的 ORIG 叶子（无报告者均以 objdump
// 反汇编转写并在注释标注地址；全部符号经 ORIG nm 核实存在）：
//   - CUser::GetServerGroupToPvP                     T 0x0864a97c
//   - DeliveryMsg C2/setBaseData                     W 0x086978c0 / 0x086978ec
//   - BuyItemDeliveryMsg C2                          W 0x0869795a
//   - LevelUpRewardItemTableKey(int,int,int)         W 0x081b8896
//   - stLevelUpRewardItemScript::get_data            W 0x081b89de（无报告，objdump）
//   - stLevelUpRewardItemScript::get_data_when_growup W 0x081b895a（无报告，objdump）
//   - CLevelRewardTable::GetRewardItemNo/GetItemCount/GetMailTitle/GetMailString
//                                                    W 0x086976f6/0x08697712/0x08697730/0x08697750（无报告，objdump）
//   - CLevelRewardTableMgr::IsOn                     W 0x08697792（无报告，objdump）
//   - CLevelRewardTableMgr::IsChallengingJob         T 0x08687792（有报告 docs/class_func_reports/CLevelRewardTableMgr/IsChallengingJob.md）
//   - CLevelRewardTableMgr::GetProperLevelRewardTableR T 0x08687e62（有报告 GetProperLevelRewardTableR.md）
//   - NewAccountLevelUpEventToJob::processLevelUpToJob T 0x081b82a4（有报告
//     NewAccountLevelUpEventToJob/processLevelUpToJob.md；定义已迁 NewAccountLevelUpEventToJob.cpp）
//   - LevelupSupportEventManger::CheckNewStep        T 0x08147164（无报告，objdump）
//   - LevelupSupportEventManger::UpdateEventInfo     T 0x081471c6（无报告，objdump）
//   - LevelupSupportEventManger::SendCharacState     T 0x0814701e（无报告，objdump；静态，操作 user->m_levelupEvents）
//   - LevelupSupport2ndEventManger::Process          T 0x08147880（无报告，objdump）
//   - DB_SaveEventLevelupSupport::makeRequest        T 0x0844cc2e（无报告，objdump）
//   - DB_EventAccountCharacterLevelUpOnceGift::makeRequest T 0x08449f66（无报告，objdump）
//
// [S426 单文件泳道 2026-09-09 复验/翻正] 清单 7 件：
//   * _postQuestTriggerItem      DIFF→AE：else 内层 if 改 P223 负条件 goto 形，
//     复现 ORIG 0x08668bfd `je <nop 岛>` + body 尾 `jmp <join>` + nop（203=203）。
//   * GetRewardItemNo/GetItemCount  [b28d-PoolSmall 归零] S426 的「P231 纯 register
//     形参」旋钮实为**反效果**：register 令 4.4 把 jobIdx→%edx / levelIdx→%eax，
//     this 再要 %eax ⇒ 多 `mov %edx,%ecx; mov %eax,%edx` 两条 regmove（13 条 DIFF）。
//     **删 register** 后 4.4 自然分配 job→%ecx / lv→%edx / this→%eax，两件各
//     11=11 **IDENTICAL**（ORIG 0x086976f6 / 0x08697712）⇒ TU diff 5→3。
//     ⇒ 补 P248「裸 register」反例：二维下标 `m[j][l]` 形参加 register 恒退化。
//   * processLevelUpEventReward  [L29] DIFF→AE：真因是**实参求值序**（旧 union +
//     reinterpret_cast<MSG_MAILBOX_SEND*>(history.raw) 的 cast 令 4.4 先物化
//     实参再 add 算 this）；改 MSG_MAILBOX_SEND 真类型局部 + `&history` 直传即
//     复现 ORIG 的 this-first+lea（旧判 N107 取址 lea-vs-add **作废**）。
//   * eventDeliveryBuyItem       [L29] NEAR→AE：裸 `register` 具名 serverGroup
//     复现 ORIG callee-saved 角色（旧判 N7 封顶**作废**）。
//   * _processLevelReward        [L29] 维持 **N121**：尾部 return 中继岛 7 vs 4
//     （4.4 恒为每个 return 生桩；4.1.2 -m32 对同构骨架 0 桩）⇒ 跨版本 CFG 出口
//     块分配差，穷举 7 形 + 4 组旗标不可翻。帧/槽/指令数全等，语义等价。
//   * UpdateEventInfo            [L29] 维持 **N7/P75**：uid/characNo 装载寄存器
//     （ORIG characNo→%eax、我们 →%edx）；register/unsigned 两形实测均打成 DIFF。
//   * _Z41__static_initialization_and_destruction_0ii  [L29] **N123 撞名假象**：
//     本 TU GI 与 ORIG 0x0869376b 副本逐条同构（compare_tu 按名索引取到他 TU 的
//     0x0807ee76 副本）⇒ 结构性噪音，verdict 豁免。
//   L29 收口四数：identical=74 ae=107 near=1 diff=2（p167 七符号全 CLEAR）；
//   无 .h 改动（仅本 TU 顶部补 `#include "PacketTypes.h"`）；无子子代理。
// ============================================================================

#include <string.h>
#include <stdlib.h>

#include "CUser.h"                    // CUser/cUserHistoryLog/m_checkSumSeed/
                                      // m_historyLog/m_codeHackCheck/getCharacLevelUpGift_AccountOnce
#include "CUserCharacInfo.h"          // getCurCharacR/getPVPResultR/getCurCharacInvenR/
                                      // getCurCharacSkillR/getCurCharacNo/get_charac_job/
                                      // get_charac_level/getCurCharFirstGrowType/PvpResultType
#include "Quest.h"                    // Quest 完整类型（m_mailTitle +0x10c /
                                      // m_mailContents +0x110 / m_mailItems +0x114）
#include "Inven_Item.h"               // Inven_Item（m_field1/m_addInfo/m_addInfo2/m_fieldb）
#include "CInventory.h"               // makeCheckSum(int,int) const / OS_API::snprintf
#include "SkillSlot.h"                // SkillSlot::makeCheckSum(int) const
#include "CDataManager.h"             // G_CDataManager/find_item/m_levelUpReward/getItemList
#include "DataManagerInitDepends.h"   // CLevelRewardTable/CLevelRewardTableMgr
#include "CGuildServerProxy.h"        // CEventManager/CEventBase::IsEventing/DeliveryMsg/
                                      // BuyItemDeliveryMsg
#include "NewAccountLevelUpEventToJob.h"  // processLevelUpEvent 调用的完整类
#include "GlobalData.h"               // s_event_manager/s_stream_pool/s_msgq_mgr/s_GM_Manager
#include "LevelupSupportEventManger.h"
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CItemList.h"                // create_item/ENUM_ITEM_CREATE_TYPE
#include "CItem.h"                    // CItem::make_item/get_index
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf clear/put_*/finalize）
#include "CStreamGuard.h"             // CStreamGuard（operator*/operator<<）
#include "PacketTypes.h"              // MSG_MAILBOX_SEND 完整定义（sizeof 0x145，
                                      // m_name11@+0x11/m_itemId39@+0x39/m_extra3d@+0x3d）
#include "MsgQueueMgr.h"              // MsgQueueMgr::put/DB_Q
#include "DB_SaveEventLevelupSupport.h"
#include "DB_EventAccountCharacterLevelUpOnceGift.h"
#include "GameWorld.h"                // G_GameWorld/IsIntegratedPvPBaseChannel
#include "CEnvironment.h"             // G_CEnvironment/m_serverEnvir.m_serverGroup
#include "CGM_Manager.h"              // CGM_Manager::GetCurrentGmMode
#include "CParty.h"                   // CParty::Reset_party_overlapped_drop_ratio
#include "CExpandEquipslot.h"         // CExpandEquipslot::Change_Equip
#include "HeroMissionEvent.h"         // HeroMissionEvent::processMission/MissionNo

// ----------------------------------------------------------------------------
// 1. ORIG T 0x0865f382 sz 0x291。
// space==3：slot∈[10,0x15] 校验 → dstSpace!=0 拒绝 → 交易中拒(0x13) →
// change_equip(dstSpace,(short)dstSlot,slot)，成功写 *outSlot=slot。
// space==0x12：dstSpace!=0 拒绝 → 交易中拒(0x13) → 扩展槽 Change_Equip(this,
// dstSpace,dstSlot,space,slot)，成功写 *outSlot=dstSlot。
// space==0 且 dstSpace==3：dstSlot∈[10,0x15] 与 slot∈[3,0x38] 双重校验 → 交易
// 中拒(0x13) → change_equip(space,(short)slot,dstSlot)，成功写 *outSlot=
// dstSlot 并对队伍重算重叠掉率。space==0 且 dstSpace==0x12：交易中拒(0x13)
// → slot∈[3,0x38] 校验 → Change_Equip(this,space,slot,dstSpace,dstSlot)，
// 成功写 *outSlot=dstSlot。其余返回 1。
int CUser::change_equip_expand(int space, int slot, int dstSpace,
                               int dstSlot, int* outSlot)
{
    // b28d-solo-CUserDepsB: AE（189=189，本轮由 DIFF 翻正）——三条本轮定案：
    // ①P46 叶块排放序 = case **书写**顺序 ⇒ 外层 switch 必须写 3 → 0 → 0x12
    //   （原写 3 → 0x12 → 0，0x12 体整段前移 37 行；决策链 3/0x12/0 与书写序无关）；
    // ②P286 单 case switch 复现 `mov 0x14(%ebp),%eax; test %eax,%eax; jne`
    //   （if 形 4.4 恒折 `cmpl $0,mem`；两处 dstSpace 守卫均改
    //   `switch(dstSpace){case 0:{主体} default:return 1;}`）；
    // ③内层 default 直接 `return 1;` 复现 ORIG 0x0865f57e 独立出口。
    // P17/P208 外层/内层 switch 决策链 + ret 提为函数级 -0x18 槽照旧。ORIG 0x0865f382。
    int ret;                               // ORIG 各 case 共用 -0x18 槽（无初值）
    switch (space)
    {
    case 3:
    {
        if (slot <= 9 || slot > 0x15)      // cmpl $0x9 jle / cmpl $0x15 jle
            return 1;
        // P286：ORIG 0x0865f3bc 是 `mov 0x14(%ebp),%eax; test %eax,%eax; jne`
        // （形参载入+寄存器比较）；if 形在 4.4 恒折 `cmpl $0,mem` ⇒ 单 case
        // switch 复现载入形。
        switch (dstSpace)
        {
        case 0:
        {
            if (CheckInTrade())            // W 0x080da2fe
                return 0x13;
            // call 0x0865eed2：arg1=dstSpace(0x14(%ebp))、arg2=(short)dstSlot(cwtl)、
            // arg3=slot(0x10(%ebp))。
            ret = change_equip(dstSpace, static_cast<short>(dstSlot), slot);
            if (ret != 0)
                return ret;
            *outSlot = slot;               // mov 0x18(%ebp) → *0x1c(%ebp)
            return 0;
        }
        default:
            return 1;
        }
    }
    case 0:
    {
        // ORIG 0x0865f42a 段同为 switch 决策链（`mov 0x14(%ebp),%eax;
        // cmp $0x3,%eax; je; cmp $0x12,%eax; je; jmp default`）。
        switch (dstSpace)
        {
        case 3:
        {
            if (dstSlot <= 9 || dstSlot > 0x15)
                return 1;
            if (slot <= 2 || slot > 0x38)
                return 1;
            if (CheckInTrade())
                return 0x13;
            // call 0x0865eed2：arg1=space(0xc(%ebp))、arg2=(short)slot、arg3=dstSlot。
            ret = change_equip(space, static_cast<short>(slot), dstSlot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            // call 0x0865514c GetParty + 非空调 Reset_party_overlapped_drop_ratio。
            CParty* party = static_cast<CParty*>(GetParty());
            if (party != 0)
                party->Reset_party_overlapped_drop_ratio();
            return 0;
        }
        case 0x12:
        {
            if (CheckInTrade())            // 先交易校验（+0x166 处先于区间检查）
                return 0x13;
            if (slot <= 2 || slot > 0x38)
                return 1;
            CExpandEquipslot* expand =
                GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
            // call 0x08499a48：(expand, this, p1, p2, p3, p4)。
            ret = expand->Change_Equip(this, space, slot, dstSpace, dstSlot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            return 0;
        }
        default:
            return 1;      // ORIG 0x0865f57e 独立出口（mov$1;jmp epilogue）
        }
    }
    // ORIG 叶块排放序 = 3 → 0 → 0x12（P46：叶块顺序 = case 书写顺序；决策链
    // 顺序 3/0x12/0 与书写序无关）。原把 0x12 写在 0 前 ⇒ 0x12 体整段前移 37 行。
    case 0x12:
    {
        // 同 case 3：P286 单 case switch 复现 `mov 0x14(%ebp),%eax; test;
        // jne`（ORIG 0x0865f4e8 段）。
        switch (dstSpace)
        {
        case 0:
        {
            if (CheckInTrade())
                return 0x13;
            // GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9)：W 0x080dd584，
            // 字面量 9 由 movl $0x9 压栈。
            CExpandEquipslot* expand =
                GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
            // call 0x08499a48：(expand, this, p3, p4, p1, p2)。
            ret = expand->Change_Equip(this, dstSpace, dstSlot, space, slot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            return 0;
        }
        default:
            return 1;
        }
    }
    default:
        break;
    }
    return 1;
}

// ----------------------------------------------------------------------------
// 2. ORIG T 0x0866463c sz 0x85。
// 构造 BuyItemDeliveryMsg（W 0x0869795a），setBaseData(2, accId, characNo,
// serverGroupToPvP) 后填 +0x10 deliveryType、+0x14 &item，经
// CEventManager::eventDeliveryNotify(msg.m_type, msg) 转发（首参取消息体
// 首字段）。
void CUser::eventDeliveryBuyItem(int deliveryType, const Inven_Item& item)
{
    // b28d-solo-CUserDepsB: NEAR——P23/P24 实参内联（去三具名槽，帧 0x58→0x40、
    // 42=42 指令全等）；残差 N7：第 3/第 2 实参 callee-saved 角色互换（ORIG
    // serverGroup→%esi、characNo→%ebx；4.4 →%ebx、%esi），探针同构形给 ORIG 分配、
    // 本 TU 因可见 GetServerGroupToPvP 定义而反转，源形不可择。ORIG 0x0866463c。
    // [S191 本轮复验] p167 CLEAR（6/6）；42=42 仅两处 `mov %eax,%esi/%ebx` 与
    // 对应压栈寄存器互换（P248② 反例族）。试形：cast/具名中间量/求值序均不可翻
    // ⇒ N7 封顶（助记符序列全等，仅 callee-saved 角色）。ORIG 0x0866463c。
    // [L29 2026-09-09 翻正 NEAR→AE] 旧判「N7 封顶」**作废**：给 serverGroup 补
    // **裸 `register` 具名局部**（无 asm 绑定，P0 合规）即复现 ORIG 的 callee-saved
    // 角色（serverGroup→%esi、characNo→%ebx；旧直内联形 4.4 恒 %ebx/%esi 互换）。
    // 边界：同法在 UpdateEventInfo（uid/characNo 加 register 或改 unsigned）实测
    // 均**打成 DIFF** ⇒ register 旋钮按件实测，非普遍解。ORIG 0x0866463c。
    BuyItemDeliveryMsg msg;                        // lea -0x20(%ebp) ctor
    register int serverGroup = GetServerGroupToPvP();   // P231 裸 register（翻正件）
    msg.setBaseData(2, get_acc_id(), getCurCharacNo(),
                    serverGroup);                  // W 0x086978ec
    msg.m_deliveryType = deliveryType;             // -0x10(%ebp) → +0x10
    msg.m_item = &item;                            // -0xc(%ebp) → +0x14
    GlobalData::s_event_manager->eventDeliveryNotify(
        msg.m_type, msg);                          // T 0x08116874，首参取 msg 首 dword
}

// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868745e sz 0x269。
// 等级奖励邮件：CLevelRewardTableMgr 开启且表就绪、非挑战职业时按 (jobIdx,
// levelIdx) 取 itemNo/count，组 Inven_Item 后经 ReqDBSendNewSystemMail 投递
// （itemNo==0 仅金额走第三参；==1 固定道具直写字段后 ResetItemAttr；其余经
// find_item 的虚槽 2 make_item 复制并 set_add_info）。
void CUser::_processLevelReward(unsigned int level, unsigned int characNo,
                                char job)
{
    // b28d-solo-CUserDepsB: 语义全还原（含两处真 bug 修正：①IsChallengingJob
    // 判定反——ORIG 0x086874cb/Ghidra C 均 `if(!f()) return`；②else 分支
    // m_addInfo 写序在 make_item 之前，ORIG 0x0868761a）。
    // [SweepCUserDepsB 本轮翻正] 旧判「N95 局部槽基差 4 封顶」**作废**：
    // 真因是本函数尾部把 GetServerGroup() 存进具名局部 `ENUM_SERVER_GROUP
    // serverGroup`（多占 -0x10 槽 ⇒ 其后全部标量槽号整体 -4 平移，伪 N95）。
    // ORIG 0x086875xx 段 `mov %eax,%ebx` 证明 serverGroup 走 callee-saved
    // 跨 strlen 保活、**无槽往返** ⇒ 按 P0/P329 内联回实参位后：
    // 160(ORIG) vs 163，全函数 0..155 行逐条全等（槽号 -0x28/-0x24/-0x20/
    // -0x1c/-0x18/-0x14/-0x10/-0xc/-0x7a/-0x3d/-0x17a 与 ORIG 完全一致）。
    // [F3b-C05③ 2026-09-10 翻案 ⇒ AE（164=164 逐条全等）] 旧「N121 尾部
    // return 中继岛封顶」判词**作废**：L29 命中的 3 处 REAL（ORIG 0x086874a8
    // table==0 / 0x08687508 levelIdx<0 / 0x08687512 jobIdx<0 三条
    // `je/js <epilogue +0x260>` 直达）不是 4.1/4.4 出口块分配差，而是平铺
    // `if (c) return;` 在 4.4 各产一组 nop+jmp 中继岛。改为**正向包裹**
    // （`if (table != 0) { … }` / `if (levelIdx >= 0 && jobIdx >= 0) { … }`）
    // 后 4.4 把 fail 路径 jump-thread 直达共享尾出口 ⇒ 尾岛 7→4、ours
    // 175→164（= ORIG 164）、本件 DIFF→AE。其余 4 个 return（IsOn/
    // IsChallengingJob/itemCount/script）与 ORIG 同为岛，保持原形不动。
    // 旧探针「嵌套正向包裹=0 桩 / 掉 xor 物化」作废的原因：当时把
    // `if (!mgr->IsOn()) return;`（含 xor $1 物化条）也一并嵌套（越界改写）。
    // p167 CLEAR（20/20，仅 C1/C2 同址别名噪音）。ORIG 0x0868745e。
    CLevelRewardTableMgr* mgr = CLevelRewardTableMgr::GetInstance();
    CLevelRewardTable* table = 0;                  // local_28 初值 0
    if (!mgr->IsOn())   // P292 档①：IsOn 已按调用点 xor 形改 bool 返回（P9 直返形
                        // 不变），此处直接产 ORIG 的 xor $1;test;jne。
        return;
    table = mgr->GetProperLevelRewardTableR();     // T 0x08687e62
    // [F3b-C05③ 2026-09-10] ORIG 0x086874a8 `je <epilogue +0x260>` 直达尾 ⇒
    // 平铺 `if (table == 0) return;` 会产一组 nop+jmp 中继岛；改正向包裹复现。
    if (table != 0)
    {
        int rewardItemNo = 0;                      // ORIG -0x20 初值 0（声明在
                                                   // IsChallengingJob 之前）
        // 真语义 bug 修正：ORIG 0x086874cb `call IsChallengingJob; xor $1; test; jne
        // 早退岛` 与 Ghidra C `if (cVar2 == 1) { 主体 }` 均表明返回**真**才继续；
        // 旧写 `if (IsChallengingJob(job)) return;` 语义反。
        if (!mgr->IsChallengingJob(job))           // T 0x08687792
            return;
        int jobIdx = table->GetProperJobIndex(job);    // T 0x08687e78
        int levelIdx = table->GetProperLevelIndex(level);  // T 0x086876c8
        // [F3b-C05③ 2026-09-10] ORIG 0x08687508/0x08687512 两条 `js <epilogue
        // +0x260>` 直达同一尾标签（相邻同目标）⇒ 正向 && 包裹复现；平铺两条
        // `if (…) return;` 会各产一组 nop+jmp 中继岛（+4 条）。
        if (levelIdx >= 0 && jobIdx >= 0)
        {
            rewardItemNo = table->GetRewardItemNo(jobIdx, levelIdx);  // local_24
            int itemCount = table->GetItemCount(jobIdx, levelIdx);    // local_18
            if (itemCount < 0)
                return;
            char title[21];                            // local_41（21 字节槽）
            strncpy(title, table->GetMailTitle(levelIdx), 0x14);
            char body[256];                            // local_17e
            strncpy(body, table->GetMailString(levelIdx), 0xff);
            Inven_Item item;                           // local_7e（W ctor）
            int sendCount = 0;                         // local_14
            if (rewardItemNo == 0)
            {
                sendCount = itemCount;                 // 仅金额，道具保持默认构造
            }
            else if (rewardItemNo == 1)
            {
                // ORIG 0x086875d5..0x086875ec 写序：+2=1（立即数，条件已证 ==1）、
                // +1=2、+7=itemCount、+11=1(word)。
                item.m_addInfo = 1;
                item.m_field1 = 2;
                item.m_addInfo2 = itemCount;
                item.m_fieldb = 1;
                item.ResetItemAttr();                  // W 0x080cb892
            }
            else
            {
                // P231 纯 register（无 asm 绑定，P0 合规）：ORIG 0x086875f9
                // `mov -0x20(%ebp),%ebx` 跨 G_CDataManager 调用保活（callee-saved）；
                // 直接内联实参 4.4 用 %edx 中转且不 push %ebx（全帧浅 4）。
                register int itemNo = rewardItemNo;    // P231：值跨 G_CDataManager
                                                       // 调用保活于 %ebx（无绑定）
                CItem* script = G_CDataManager()->find_item(itemNo);  // T 0x0835fa32
                if (script == 0)
                    return;                            // je +0x25f 直接出口
                item.m_addInfo = rewardItemNo;         // ORIG 0x0868761a 先写再 make
                script->make_item(item);               // vtable+8 虚槽（CItem 槽 2）
                item.set_add_info(itemCount);          // W 0x080cb884
            }
            // [P0/P329 本轮] ORIG 0x086875xx 段把 GetServerGroup() 结果留 %ebx 跨
            // strlen 保活（行 131 `mov %eax,%ebx`），无具名局部槽 ⇒ 原码为实参位内联
            // 表达式；具名 `ENUM_SERVER_GROUP serverGroup` 多占 -0x10 槽并使全帧标量
            // 槽号整体 -4 平移（N95 假象）。
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, sendCount, characNo, body,
                static_cast<int>(strlen(body)), 0, GetServerGroup(), 0, 0);
        }
    }
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x086689ee sz 0x32a。
// Quest 触发物品邮寄：m_mailItems(+0x114) 按 (itemNo,count) 成对遍历；
// itemNo==0 且 count>0 走金额路径（count 入第三参）；否则 find_item 经
// make_item 组道具（count 写 m_addInfo2，第三参置 0）。空列表时寄一封默认
// 道具邮件。标题/正文取 m_mailTitle(+0x10c)/m_mailContents(+0x110)。
void CUser::_postQuestTriggerItem(const Quest* quest)
{
    // b28d-solo-CUserDepsB: ①P47/P222 极性反转——ORIG 0x086689ee 先
    // `call empty; test; je <非空分支>`，空列表分支在前（原 `if(!empty)` 反向）；
    // ②P23/P44 实参内联右到左求值（ORIG edi=GetServerGroup、-0xb8=size、
    // esi=body.c_str、ebx=getCurCharacNo、title.c_str 即时），去 5 个具名槽；
    // ③去 items 引用局部（ORIG 每处重读 quest->m_mailItems）。
    // [S426 本轮翻正 ⇒ AE（203=203 逐条同构）] ④第二对元素读完后才 `++it`
    // （循环尾公共块，ORIG 0x08668cce）；⑤else 内层 `if (script != 0) { body }`
    // 改写 P223 负条件 goto 形：ORIG 0x08668bfd `cmpl $0,-0x1c(%ebp);
    // je <nop 岛 +0x2df>` + body 尾 0x08668ccb `jmp <join +0x2e0>` + nop 是
    // 4.1 的 if-join 岛，4.4 对正向 if 恒 jump-thread（少 jmp+nop）；改
    // `if (script == 0) goto post_quest_item_next; body; post_quest_item_next:;`
    // 后 4.4 精确产 `je <nop>; body; jmp <join>; nop; join:`（探针
    // /tmp/b28d_s426/probe_goto.cpp f1 复证）。旧「N112/N121 封顶」判词作废。
    // p167 CLEAR（30/30）。ORIG 0x086689ee。
    if (quest->m_mailItems.empty())
    {
        Inven_Item mailItem;                       // 空列表单封默认道具邮件
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            quest->m_mailTitle.c_str(), mailItem, 0, getCurCharacNo(),
            quest->m_mailContents.c_str(),
            static_cast<int>(quest->m_mailContents.size()), 0,
            GetServerGroup(), 0, 0);
    }
    else
    {
        // 迭代器成对读取（读 A→++→读 B→处理→++），与 ORIG 循环形态一致。
        for (std::vector<int>::const_iterator it = quest->m_mailItems.begin();
             it != quest->m_mailItems.end(); )
        {
            int itemNo = *it;
            ++it;
            int count = *it;
            Inven_Item mailItem;                   // local_aa 每对新建
            if (itemNo == 0 && count > 0)
            {
                mailItem.m_addInfo = itemNo;       // -0xa4
                mailItem.m_addInfo2 = count;       // -0x9f
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    quest->m_mailTitle.c_str(), mailItem, count,
                    getCurCharacNo(), quest->m_mailContents.c_str(),
                    static_cast<int>(quest->m_mailContents.size()), 0,
                    GetServerGroup(), 0, 0);
            }
            else
            {
                CItem* script = G_CDataManager()->find_item(itemNo);
                // P223（本轮翻正）：ORIG 0x08668bfd `cmpl $0,-0x1c(%ebp);
                // je <nop 岛 +0x2df>` + body 尾 0x08668ccb `jmp <join +0x2e0>`
                // + nop ⇒ 4.4 唯一复现形 = 负条件 goto 到 else 块尾空标签
                // （`if (script != 0) { body }` 被 jump-thread 成无 jmp/nop）。
                if (script == 0)
                    goto post_quest_item_next;
                script->make_item(mailItem);   // vtable+8
                mailItem.m_addInfo = itemNo;
                mailItem.m_addInfo2 = count;
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    quest->m_mailTitle.c_str(), mailItem, 0,
                    getCurCharacNo(), quest->m_mailContents.c_str(),
                    static_cast<int>(quest->m_mailContents.size()), 0,
                    GetServerGroup(), 0, 0);
            post_quest_item_next:;
            }
            // ORIG 0x08668cce 段：第二对元素处理完后才 `++it`（循环尾公共块），
            // 非紧跟 `int count = *it;`（P28 增量侧：位置决定 ppEv 落点）。
            ++it;
        }
    }
}

// ----------------------------------------------------------------------------
// 5. ORIG T 0x08663cc0 sz 0x3b9。
// 升级奖励发放：repeat 事件 0x5d IsEventing 门控后按 (level, job, growType)
// 查 stLevelUpRewardItemScript（growup 时聚合收集，否则单项），逐条 create_item
// 并邮寄（标题 game_server_msg_187 / 正文 game_server_msg_188）；邮寄成功回执
// 为 0 时补写 MSG_MAILBOX_SEND 历史（+0x11 标题/+0x39 item idx/+0x3d add_info）
// 经 cUserHistoryLog::SendMail 落日志。
void CUser::processLevelUpEventReward(bool growupFlag)
{
    // b28d-solo-CUserDepsB: 语义闭合（本轮 256→246，p167 归零）——①P23/P200 key
    // 构造实参内联（grow→%esi/job→%ebx/level→%eax，去三槽）；②P214③
    // `char body[256]={0}` 复现 ORIG 0x086640c0 rep stos 内联清零（memset 恒 call）；
    // ③[本轮] P167 调用点差集归零：`(*it)` 具名引用局部（ORIG 0x08663e92 deEv 存
    // -0x1c 槽）+ `G_CDataManager()->m_itemList` 成员直读（原 getItemList() 多 1 call）；
    // ④[本轮] P0 四值实参内联（serverGroup/bodyLen/characNo/mailTitle 原各占 4 槽，
    // 致全帧 +0x10；ORIG 走 %esi/%edi/%ebx/%eax 无槽）⇒ 帧 0x2fc 与 ORIG 全等；
    // ⑤history 写入：权威类型 MSG_MAILBOX_SEND 局部（POD，sizeof 0x145）复现
    // 常量 disp 直存。
    // [L29 2026-09-09 翻正 DIFF→AE] 旧判「N107 成员取址 lea-vs-add 封顶」**作废**：
    // 真因是**实参求值序**——旧形 union + `reinterpret_cast<MSG_MAILBOX_SEND*>(
    // history.raw)` 的 cast 令 4.4 先物化实参（`lea -0x2c2(%ebp),%eax`）再以
    // `add $0x79700,%edx` 算 this；改真实类型局部 + `&history` 直传（无 cast、
    // 类型精确匹配形参）后 4.4 与 ORIG 同为 `mov 0x8(%ebp),%eax;
    // lea 0x79700(%eax),%edx`（this-first + lea）。探针 /tmp/l29_p3.cpp m6
    // （无 cast）=ORIG 形 / m10（含 cast）=旧形 逐条对照。配套：TU 顶部
    // `#include "PacketTypes.h"`（完整类型；m_name11@+0x11/m_itemId39@+0x39/
    // m_extra3d@+0x3d 经权威头核对）。ORIG 0x08663cc0。
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (event->IsEventing(0) != true)              // vtable+0x34（槽 13）==1 继续
        return;
    // P23/P200 实参内联右到左求值：ORIG 0x0866407f 段 grow→%esi、job→%ebx、
    // level→%eax 直压（无具名槽）。
    LevelUpRewardItemTableKey key(get_charac_level(), get_charac_job(),
                                  getCurCharFirstGrowType());  // W 0x081b8896
    std::vector<LevelUpRewardItemData> rewards;    // vector C1 0x081b8c00
    if (growupFlag)
    {
        if (!G_CDataManager()->m_levelUpReward.get_data_when_growup(key, rewards))
            return;                                // !=1 清理后出口
    }
    else
    {
        LevelUpRewardItemData data;                // C1 0x081b88bc
        if (!G_CDataManager()->m_levelUpReward.get_data(key, data))
            return;
        rewards.push_back(data);                   // push_back 0x081b8a52
    }
    char body[256] = {0};                          // P214③：ORIG 0x086640c0 段
                                                   // rep stos 内联清零（memset 恒 call）
    strncpy(body, g_scriptStringManager_.findString(
                      4, "game_server_msg_188", (bool*)0),
             0xff);
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        // P167 新判据：ORIG 0x08663e92 调 `operator*`(deEv) 并把结果存 -0x1c 槽
        // （P0 具名引用局部），随后两次字段读取均从该槽解引用；我方旧写
        // `it->m_field0` 发 2 次 `operator->`(ptEv) ⇒ MISSING deEv/EXTRA ptEv。
        LevelUpRewardItemData& data = *it;
        Inven_Item item;                           // local_81（ctor 0x080cb854）
        item.m_addInfo = data.m_field0;            // -0x7b ← data.f0（create_item 前）
        // ORIG 0x08663eb3 段：`call G_CDataManager; mov 0xc(%eax),%eax` 是**成员直读**
        // （CDataManager+0xc = m_itemList），非 getItemList() 调用（P167 EXTRA）。
        G_CDataManager()->m_itemList->create_item(
            ENUM_ITEM_CREATE_TYPE_0, item, data.m_field4);  // T 0x08511fb8
        // P0：ORIG 0x08663ed6..0x08663f64 四值全走 callee-saved 直传（serverGroup
        // →%esi、bodyLen→%edi、characNo→%ebx、mailTitle→%eax 末位），无任何栈槽
        // ⇒ 原码是实参内联右到左求值（具名局部各占 4 槽并致全帧 +0x10）。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_187",
                                                  (bool*)0),
                item, 0, getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0, GetServerGroup(),
                0, 0) == 0)
        {
            // [L29 翻正] ORIG 0x08663f76..0x08664005 实证 history 是
            // **MSG_MAILBOX_SEND 局部**（sizeof 0x145，POD：memset 后无 ctor，
            // MSG_BASE 仅 char m_head[0xd]）：①`memset(&history,0,0x145)`；
            // ②strncpy 目标 `lea -0x2c2(%ebp); add $0x11`（成员数组名退化）；
            // ③+0x39/+0x3d 常量 disp 直存（m_itemId39/m_extra3d）；
            // ④`lea 0x79700(%eax),%edx`（this-first）+`lea &history` 直传。
            // 旧 union+reinterpret_cast 形令 4.4 先算实参（lea）再 `add $0x79700`
            // 算 this ⇒ 与 ORIG 反序并折 add。
            MSG_MAILBOX_SEND history;              // 基址 -0x2c2（pack(1)，0x145）
            memset(&history, 0, sizeof(history));
            strncpy(history.m_name11,              // +0x11
                    g_scriptStringManager_.findString(4, "game_server_msg_187",
                                                      (bool*)0),
                    0x14);
            history.m_itemId39 = item.m_addInfo;   // +0x39
            history.m_extra3d = item.m_addInfo2;   // +0x3d
            m_historyLog.SendMail(&history, 0);    // T 0x086847ea
        }
    }
}

// ----------------------------------------------------------------------------
// 6. ORIG T 0x0869115a sz 0x113。
// 升级事件分发：repeat 0x68 IsEventing 门控后二次取事件调
// processLevelUpToJob(user,false)；0x9e → LevelupSupportEventManger::
// UpdateEventInfo；0xa3 → LevelupSupport2ndEventManger::Process；最后无条件
// HeroMissionEvent::processMission(user, MissionNo 4, 0)。
void CUser::processLevelUpEvent()
{
    // b28d-solo-CUserDepsB: AE——P0/P183 去缓存：ORIG 0x08691160 每次都重新
    // `mov &s_event_manager,%eax` 并把 GetRepeatEvent 结果留在 %eax 直接做虚调用
    // （无 mgr/event 具名槽）；仅第二段判空有 -0xc 槽（原码具名 levelUpToJob）。
    // 71=71 逐条同构。ORIG 0x0869115a。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x68)->IsEventing(0))
    {
        // 二次取同一事件（ORIG 再次 GetRepeatEvent(0x68)），判空后调用。
        NewAccountLevelUpEventToJob* levelUpToJob =
            reinterpret_cast<NewAccountLevelUpEventToJob*>(
                GlobalData::s_event_manager->GetRepeatEvent(0x68));
        if (levelUpToJob != 0)
            levelUpToJob->processLevelUpToJob(*this, false);  // T 0x081b82a4
    }
    if (GlobalData::s_event_manager->GetRepeatEvent(0x9e)->IsEventing(0))
        ARAD::Singleton<LevelupSupportEventManger>::Get()->UpdateEventInfo(this);
    if (GlobalData::s_event_manager->GetRepeatEvent(0xa3)->IsEventing(0))
        ARAD::Singleton<LevelupSupport2ndEventManger>::Get()->Process(this);
    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, HeroMissionCondition::MissionNo::T_4, 0);       // T 0x081672be
}

// ----------------------------------------------------------------------------
// 7. ORIG T 0x0864e0b6 sz 0x5d。
// 无当前角色返回 0；否则 makeCheckSum(pvpWinPoint(+0x58), m_checkSumSeed(+0x796e0))，
// 结果即 makeCheckSum 尾部 eax（CInventory::makeCheckSum 已改为 int 以复现）。
int CUser::getCurCharacInvenCheckSum() const
{
    if (getCurCharacR() == 0)                      // W 0x08120432
        return 0;
    // b28d-solo-CUserDepsB: AE——P52/P0 单语句链式实参（两实参升格 %esi/%ebx 跨
    // receiver 调用保活）+ P248 档③字段类型旋钮：m_checkSumSeed 由 int 改
    // unsigned int（Main 代落，连同 m_backupSeed）后复现 ORIG 0x0864e0e2 两段
    // `mov 0x796e0(%eax),%eax; mov %eax,%esi`（int 字段 4.4 恒折单段）。
    return getCurCharacInvenR()->makeCheckSum(getPVPResultR()->m_winPoint,
                                              m_checkSumSeed);   // T 0x0850ae7a
}

// ----------------------------------------------------------------------------
// 8. ORIG T 0x0864e114 sz 0x49。
// 同上种子；对当前角色 SkillSlot 调 makeCheckSum(seed)。
int CUser::getCurCharacSkillCheckSum() const
{
    if (getCurCharacR() == 0)
        return 0;
    // b28d-solo-CUserDepsB: AE——P52/P0 单语句链式实参（seed 升格 %ebx 跨
    // getCurCharacSkillR 保活）+ P248 档③字段 unsigned 化后复现 ORIG 0x0864e139
    // `mov 0x796e0(%eax),%eax; mov %eax,%ebx` 两段。
    return reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->makeCheckSum(m_checkSumSeed);            // T 0x086066f2
}

// ============================================================================
// 依赖叶子（ORIG 直调；地址与语义见文件头清单）
// ============================================================================

// ORIG T 0x0864a97c sz 0x30：PvP 统合频道取 CUser+0x79630（CodeHackCheckStorage
// +0x34 整数覆盖值），否则 CEnvironment+0x378 = m_serverEnvir.m_serverGroup。
int CUser::GetServerGroupToPvP() const
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return m_postChecksumData.m_serverGroup;
    return G_CEnvironment()->m_serverEnvir.m_serverGroup;
}

// DeliveryMsg::DeliveryMsg（ORIG W 0x086978c0，四字段清零）定义在
// CtorProducers_1.cpp（NL4 unit-at-a-time 塌缩机制：同 TU 可见 callee 使
// BuyItemDeliveryMsgC1 outgoing 帧塌缩 sub $0x4，ORIG sub $0x18）。

// ORIG W 0x086978ec：四参顺序写 +0x00..+0x0c。
void DeliveryMsg::setBaseData(int type, unsigned int accId, int characNo,
                              int serverGroup)
{
    m_type = type;
    m_accId = accId;
    m_characNo = characNo;
    m_serverGroup = serverGroup;
}

// ORIG W 0x0869795a：基类构造后清零两扩展字段。
BuyItemDeliveryMsg::BuyItemDeliveryMsg() : DeliveryMsg()
{
    m_deliveryType = 0;
    m_item = 0;
}

// ORIG W 0x081b8896：(level 截断 ushort, job 字节, grow 字节)。
LevelUpRewardItemTableKey::LevelUpRewardItemTableKey(int level, int job,
                                                     int grow)
{
    m_lv = static_cast<short>(level);   // b28c-L20: m_lv signed short（ORIG 16 位有符号比较）
    m_job = static_cast<unsigned char>(job);
    m_grow = static_cast<unsigned char>(grow);
}

// ORIG W 0x081b89de：find 命中拷贝两 int 返回 1，未命中返回 0。
bool stLevelUpRewardItemScript::get_data(const LevelUpRewardItemTableKey& key,
                                         LevelUpRewardItemData& out)
{
    // b28c-L20: AE — 槽位与 ORIG 全等（it@-0x10 find sret / end 临时@-0xc）；
    // ORIG 0x081b8a15 的 `operator==` 接收者是 end 临时（this=-0xc, other=-0x10）
    // ⇒ 源码书写序为 `end() == it`；另 ORIG 0x081b8a38 调 `operator*`(deEv) 非
    // `operator->`(ptEv) ⇒ 写 `(*it).second`（b26「槽序镜像不可翻」判词作废）。
    std::map<LevelUpRewardItemTableKey, LevelUpRewardItemData>::iterator it =
        m_map0.find(key);
    if (m_map0.end() == it)
        return false;
    out = (*it).second;                            // 两 int 顺序写（+0/+4）
    return true;
}

// ORIG W 0x081b895a：等价于嵌套 functor GetGrowUpRewardItemFunctor（W 0x081b88fe
// operator()：同 grow(+3)、同 job(+2)、pair.lv <= key.lv 才 push_back）的
// for_each（0x081b8b11）；返回 !out.empty()。
bool stLevelUpRewardItemScript::get_data_when_growup(
    const LevelUpRewardItemTableKey& key,
    std::vector<LevelUpRewardItemData>& out)
{
    // b28d-solo-CUserDepsB: AE——ORIG 0x081b895a 是 std::for_each(begin,end,
    // GetGrowUpRewardItemFunctor(key,out)) 单语句（functor sret -0x18 / end -0x10 /
    // begin -0xc / for_each 返回临时 -0x20）+ `return !out.empty()`（xor $1）；
    // 手写三条件循环多 36 条（谓词逐条物化 + 三个 jmp/nop 岛）。
    std::for_each(m_map0.begin(), m_map0.end(),
                  GetGrowUpRewardItemFunctor(key, out));
    return !out.empty();
}

// ---- CLevelRewardTable 访问器（偏移与权威布局逐指令核对）----

// ORIG W 0x086976f6：this+8+(jobIdx*0x46+levelIdx+0x44)*4 → m_rewardPrimary 下标。
int CLevelRewardTable::GetRewardItemNo(int jobIdx, int levelIdx)
{
    // [b28d-PoolSmall 归零] 旧 P231「纯 register 形参」旋钮是**反效果**（P248 裸
    // register 反例）：带 register 时 4.4 把 jobIdx 装 %edx、levelIdx 装 %eax，
    // 随后 this 又要装 %eax ⇒ 多出 `mov %edx,%ecx; mov %eax,%edx` 两条 regmove
    // （13 条 DIFF）。**删 register** 后 4.4 自然分配 job→%ecx / lv→%edx /
    // this→%eax，逐条复现 ORIG 0x086976f6 的
    // `mov 0xc(%ebp),%ecx; mov 0x10(%ebp),%edx; mov 0x8(%ebp),%eax;
    //  imul $0x46,%ecx,%ecx; lea (%ecx,%edx,1),%edx; add $0x44,%edx;
    //  mov 0x8(%eax,%edx,4),%eax` ⇒ 11=11 IDENTICAL。
    return m_rewardPrimary[jobIdx][levelIdx];   // [b28d-Main·检查点17] 二维下标（头已改 [9][0x46]）
}

// ORIG W 0x08697712：this+(jobIdx*0x46+levelIdx+0x2bc)*4 → m_rewardSecondary 下标。
int CLevelRewardTable::GetItemCount(int jobIdx, int levelIdx)
{
    // [b28d-PoolSmall 归零] 同 GetRewardItemNo：删 register 形参即逐条复现
    // ORIG 0x08697712（`imul $0x46,%ecx,%ecx; lea (%ecx,%edx,1),%edx;
    //  add $0x2bc,%edx; mov (%eax,%edx,4),%eax`，m_rewardSecondary@+0xaf0 无
    // disp）⇒ 11=11 IDENTICAL。带 register 时同样多 2 条 regmove。
    return m_rewardSecondary[jobIdx][levelIdx];   // [b28d-Main·检查点17] 同上
}

// ORIG W 0x08697730：this+0x14c8+levelIdx*21（21B/级 strncpy 槽）。
char* CLevelRewardTable::GetMailTitle(int levelIdx)
{
    return &m_nameStrings[levelIdx * 21];
}

// ORIG W 0x08697750：this+0x1a86+levelIdx*0x100（0x100/级 strncpy 槽）。
char* CLevelRewardTable::GetMailString(int levelIdx)
{
    return &m_messages[levelIdx * 0x100];
}

// ORIG W 0x08697782 sz 0xf：movzbl 0x6086(%eax) —— 读 m_jobAffection。
bool CLevelRewardTable::GetJobAffection() 
{
    return m_jobAffection;
}

// ORIG W 0x08697792 sz 0xc：movzbl (this) 直返全字（无 setne）——m_flag/返回
// 均 bool 时同形（P32 直返）；调用点 xor 形按 P292 要求 bool 返回。
bool CLevelRewardTableMgr::IsOn()
{
    return m_flag;
}

// ORIG T 0x08687792 sz 0x96：表缺失或 job∉[0,10] 返回 0；否则以 (job+1, 0) 查
// 奖励项号/数量，任一非 0 即挑战职业。
bool CLevelRewardTableMgr::IsChallengingJob(char job)
{
    CLevelRewardTable* table = GetProperLevelRewardTableR();
    if (table == 0)
        return false;
    if (job > 0xa || job < 0)
        return false;
    int itemNo = table->GetRewardItemNo(job + 1, 0);
    int count = table->GetItemCount(job + 1, 0);
    // [b28c-L29] ORIG 0x08687792 尾部：两条 `cmpl $0; jne` 各自跳同一 mov$1
    // 尾块（false 走 fall-through）——P222 两形互试定稿：正向包裹
    // `if(itemNo==0 && count==0) return false; return true;` 复现（反形
    // `if(!=0) return true` 被 4.4 折叠成 je+mov$1 fall，实测 44→46 反劣）。
    if (itemNo == 0 && count == 0)
        return false;
    return true;
}

// ORIG T 0x08687e62 sz 0xc：return m_table。
CLevelRewardTable* CLevelRewardTableMgr::GetProperLevelRewardTableR() const
{
    return m_table;
}

// ORIG T 0x08687e6e sz 0xa：mov $0x0,%eax —— 恒返回 0（参数未使用）。
CLevelRewardTable* CLevelRewardTableMgr::GetProperLevelRewardTableR(int) const
{
    return 0;
}

// ORIG T 0x08687d18 sz 0x54：遍历 m_table 指针数组（0x46 项），
// 元素为 0 → 空槽终止返回 false；元素 ==1 → 有起始奖励。
bool CLevelRewardTableMgr::IsHaveStartLevelReward()
{
    int i = 0;
    while (i <= 0x45)
    {
        if (m_table->m_levelKeys[i] == 0)
            return false;
        if (m_table->m_levelKeys[i] == 1)
            return true;
        ++i;
    }
    return false;
}

// ORIG T 0x08687d6c：size()==1 或 v[0]==0 ⇒ 全服生效；否则遍历含 serverIdx 才 true。
bool CLevelRewardTableMgr::IsOnServer(int serverIdx)
{
    // b28d-solo-CUserDepsB: AE——①P171 照抄：ORIG 0x08687d7b 短路链只有
    // `size()==1 || operator[](0)==0`（无 size()!=0 守卫，空表走 operator[] 越界，
    // 与 Ghidra C 一致）；②P96/P270 非 const iterator 消转换构造；③P28 后置 it++
    // 复现 ppEi 三槽阶梯（it -0x14 / end -0x10 / ++临时 -0xc），74=74 逐条同构。
    if (m_vec8.size() == 1 || m_vec8[0] == 0)
        return true;
    if (m_vec8.empty())
        return false;
    // P96/P270：ORIG 0x08687dd7 begin() 直写 it 槽、无 iterator→const_iterator
    // 转换构造（两处 ppEi/operator!= 均为非 const 版）⇒ 必须用可变 iterator。
    // P28 增量侧：ORIG 0x08687e24 调 `ppEi`（后置版，带独立 sret 临时槽）⇒ 写 it++。
    for (std::vector<int>::iterator it = m_vec8.begin();
         it != m_vec8.end(); it++)
    {
        if (*it == serverIdx)
            return true;
    }
    return false;
}

// ---- LevelupSupport 域 ----

// ORIG T 0x08147164 sz 0x62：level ∈ m_script.m_stepItems 即新档位可达。
bool LevelupSupportEventManger::CheckNewStep(int level)
{
    // ORIG T 0x08147164：end() 先求值（-0x10）后 find()（-0xc）⇒ 4.4 对
    // `==` 右起装载同序；比较用 operator== 后取反（test/je + mov$0/mov$1
    // 岛），故写 `if (find == end) return false; return true;`（P56/P220）。
    if (m_script.m_stepItems.find(level) == m_script.m_stepItems.end())
        return false;
    return true;
}

// ORIG T 0x081471c6 sz 0x19c。操作对象是 user->m_levelupEvents（CUser+0x8d0e4
// vector<EventInfo>，lea 0x8d0e4(%eax) 实证）：门槛达标且未完成(≠2)的档位标记
// state=1 并 DB 上报；末尾按 CheckNewStep 发送状态包（level 或 0）。
void LevelupSupportEventManger::UpdateEventInfo(CUser* user)
{
    // b28d-solo-CUserDepsB: NEAR（114=114，助记符序列全等）——①P214① 正极性
    // 包裹 `if(A||B){主流程}` 复现
    // ORIG 0x081471ec 的 mov$1/mov$0 短路物化 + 0x0814721d `test;je 尾`
    // （具名 bool pass 恒槽化 movb/-0x19；语句表达式/register 两形实测均劣化）；
    // ②循环上界是**本类 m_events**（ORIG 0x08147248 `this+0x34`）而非
    // user->m_levelupEvents（P171 照抄原码对象选择）；③P157 循环体逐次重调
    // operator[]（去具名 info 引用）；④真语义 bug 修正：makeRequest 第2/3参
    // 为 (characNo, level)——W8LT_DBTask.cpp dispatch 读流序 characNo→level→state、
    // InterDispatcher.cpp:17832 / UserMiscLink.cpp:522 两调用点同序，ORIG
    // 0x081472f2 压栈 `$1, level, characNo, uid` 实证。114=114，残差仅
    // N7/P75：uid/characNo 两实参装载时刻（ORIG `mov -0x14,%eax`(characNo) 先、
    // uid 后载；4.4 先保活 uid 于 %eax、characNo 走 %edx）。[S191 本轮复验]
    // 删 cast / uid 局部改 int 两形实测：前者无变化、后者把 NEAR 打成 DIFF
    // （movzwl %ax,%edx→%eax 破坏助记符序列）⇒ 维持 unsigned + 直传，N7/P75 封顶。
    // [L29 2026-09-09 复验] 判据① 全 TU 逐位：本函数**无** jcc/jmp 目标差；
    // 残差仅 makeRequest 压栈前 characNo 的载入寄存器（ORIG `mov -0x14(%ebp),%eax`
    // 后 `mov %eax,0x4(%esp)`；我们 `mov -0x14(%ebp),%edx` 后 `mov %edx,0x4(%esp)`），
    // 参数序/槽位/指令数全同。再试两形：`register int characNo`（免槽，NEAR→DIFF）、
    // `unsigned int characNo`（NEAR→DIFF）⇒ 两旋钮皆不可用，维持 N7 封顶。
    // ORIG 0x081471c6。
    int level = user->get_charac_level();
    // P214① 正极性包裹：复现 ORIG 0x081471ec 的短路物化岛（test/jne L_pass →
    // B 假落 mov$0）+ 0x0814721d `test %al,%al; je 函数尾`（主流程直落）。
    if (CheckNewStep(level) ||
        GlobalData::s_GM_Manager->GetCurrentGmMode(
            user->get_acc_id()))                   // T 0x08299276
    {
        unsigned int uid = user->GetUID();         // W 0x080c8c96
        int characNo = user->getCurCharacNo();
        unsigned int i = 0;
        unsigned int count = m_events.size();      // ORIG this+0x34（非 user）
        for (; i < count; ++i)
        {
            // P214① && 正极性包裹：复现 ORIG 的 mov$1/mov$0 物化岛。
            if (user->m_levelupEvents[i].m_level <= level &&
                user->m_levelupEvents[i].m_state != 2)
            {
                user->m_levelupEvents[i].m_state = 1;   // movw $0x1, 0x2
                DB_SaveEventLevelupSupport::makeRequest(
                    uid, characNo,
                    static_cast<int>(user->m_levelupEvents[i].m_level), 1);
            }
        }
        if (CheckNewStep(level))
            SendCharacState(user, level);
        else
            SendCharacState(user, 0);
    }
}

// ORIG T 0x0814701e sz 0x146（静态）。包头 0x22a：byte 数量 + 每档 (int level,
// byte state) + byte state 参数，finalize(true) 后 Send。
void LevelupSupportEventManger::SendCharacState(CUser* user, int state)
{
    // b28d-solo-CUserDepsB: AE——P13/P212：ORIG 0x08147076 先把 i 清 0（-0x10）、
    // size() 一次求值缓存到 -0xc（循环头不重调），故写「i=0 声明 + count=size()
    // 声明 + for(;i<count;)」；帧 0x30/槽位与 ORIG 全等，93=93 逐条同构。
    // ORIG 0x0814701e。
    PacketGuard packet;                            // C1 0x0858dd4c
    packet.clear();
    packet.put_header(0, 0x22a);
    packet.put_byte(static_cast<int>(user->m_levelupEvents.size()));
    unsigned int i = 0;
    unsigned int count = user->m_levelupEvents.size();
    for (; i < count; ++i)
    {
        packet.put_int(static_cast<int>(user->m_levelupEvents[i].m_level));
        packet.put_byte(static_cast<int>(user->m_levelupEvents[i].m_state));
    }
    packet.put_byte(state);
    packet.finalize(true);
    user->Send(packet);                            // T 0x086485ba
}

// ORIG T 0x08147880 sz 0x24d。
// 二档升级支持：脚本职业匹配当前角色后按等级取奖励列表（map find 于
// m_script.m_stepItems），逐项 find_item → get_index 写 item idx → make_item →
// set_add_info((short)count) → ReqDBSendNewSystemMail（第七参固定 0xf）。
void LevelupSupport2ndEventManger::Process(CUser* user)
{
    // b28d-solo-CUserDepsB: AE——P13 缓存 items->size()（ORIG 0x08147a8d 一次求值
    // 存 -0x10）+ P23/P52 find_item 实参内联（at(i).m_itemIndex 升格 %ebx 跨
    // G_CDataManager 调用，无具名槽）；164=164 逐条同构（余差为 .o 重定位占位）。
    // ORIG 0x08147880。
    if (user == 0)
        return;
    if (m_script.m_jobType != user->get_charac_job())
        return;
    int level = user->get_charac_level();
    std::map<int, std::vector<LevelupSupport2ndScript::ItemInfo>*>::iterator it =
        m_script.m_stepItems.find(level);
    if (it == m_script.m_stepItems.end())
        return;
    std::vector<LevelupSupport2ndScript::ItemInfo>* items = it->second;
    if (items == 0)
        return;
    const char* mailTitle = g_scriptStringManager_.findString(
        4, m_script.m_mailTitle.c_str(), (bool*)0);
    const char* mailContent = g_scriptStringManager_.findString(
        4, m_script.m_mailContent.c_str(), (bool*)0);
    // b28d-solo-CUserDepsB: P13 缓存 size()（ORIG 0x08147a8d 一次求值存 -0x10）
    // + P23/P52 find_item 实参内联（ORIG 0x08147a9f at(i).m_itemIndex 升格 %ebx
    // 跨 G_CDataManager 调用保活，无具名槽）。
    unsigned int i = 0;
    unsigned int count = items->size();
    for (; i < count; ++i)
    {
        // ORIG 用 vector::at(i)（_ZNKSt6vector…2atEj，边界检查）。
        CItem* script = G_CDataManager()->find_item(items->at(i).m_itemIndex);
        if (script == 0)
            continue;
        Inven_Item item;
        item.m_addInfo = script->get_index();      // -0x67（get_index 先于虚调用）
        script->make_item(item);                   // vtable+8
        item.set_add_info(
            static_cast<short>(items->at(i).m_itemCount));  // (short)cwtl
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            mailTitle, item, 0, user->getCurCharacNo(), mailContent,
            static_cast<int>(strlen(mailContent)), 0xf, user->GetServerGroup(),
            0, 0);
    }
}

// ---- DB 请求封装 ----

// ORIG T 0x0844cc2e sz 0x10a："DBThread.cpp"/0xc25a 流，头 0x34f + 四 int，
// 入 DB_Q（QUEUE_IDX 2）。流序 = (accId, characNo, level, state)：
// W8LT_DBTask.cpp DB_SaveEventLevelupSupport::dispatch 读流 characNo→level→state
// （ORIG 0x0844cafe）实证第 2/3 参语义，形参名据此修正。
void DB_SaveEventLevelupSupport::makeRequest(int accId, int characNo,
                                             int level, int state)
{
    // b28d-solo-CUserDepsB: AE（73=73，本轮由 NEAR 翻正）——ORIG 0x0844cc90/0x0844ccaa
    // 第 3/4 次 `operator<<` 的实参槽是 0xc(%ebp)(characNo) 与 0x10(%ebp)(level)：
    // 原代码按注释写的流序 `<< level; << characNo;` 与形参序相反（写流序实为
    // accId→characNo→level→state），交换两句即与 ORIG 逐条全等。p167 CLEAR（16/16）。
    // ORIG 0x0844cc2e。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc25a), true);
    **guard << 0x34f;
    **guard << accId;
    **guard << characNo;
    **guard << level;
    **guard << state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG T 0x08449f66 sz 0x158："DBThread.cpp"/0xb9d9 流，头 0x319、int -1、
// uint accId、short serverGroup、int characNo、short job、short level，入 DB_Q。
void DB_EventAccountCharacterLevelUpOnceGift::makeRequest(unsigned int accId,
                                                          short serverGroup,
                                                          int characNo,
                                                          short job, short level)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb9d9), true);
    **guard << 0x319;
    **guard << -1;
    **guard << accId;
    **guard << serverGroup;
    **guard << characNo;
    **guard << job;
    **guard << level;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
