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
    // b28d-40: N82 封顶——帧 0x38 vs 0x48 + 形参比较内存直比（P85），语义闭合。
    // ORIG 0x0865f382。
    if (space == 3)
    {
        if (slot <= 9 || slot > 0x15)      // cmpl $0x9 jle / cmpl $0x15 jle
            return 1;
        if (dstSpace != 0)                 // test dstSpace; jne → 1
            return 1;
        if (CheckInTrade())                // W 0x080da2fe
            return 0x13;
        // call 0x0865eed2：arg1=dstSpace(0x14(%ebp))、arg2=(short)dstSlot(cwtl)、
        // arg3=slot(0x10(%ebp))。
        int ret = change_equip(dstSpace, static_cast<short>(dstSlot), slot);
        if (ret != 0)
            return ret;
        *outSlot = slot;                   // mov 0x18(%ebp) → *0x1c(%ebp)
        return 0;
    }
    else if (space == 0x12)
    {
        if (dstSpace != 0)
            return 1;
        if (CheckInTrade())
            return 0x13;
        // GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9)：W 0x080dd584，
        // 字面量 9 由 movl $0x9 压栈。
        CExpandEquipslot* expand =
            GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
        // call 0x08499a48：(expand, this, p3, p4, p1, p2)。
        int ret = expand->Change_Equip(this, dstSpace, dstSlot, space, slot);
        if (ret != 0)
            return ret;
        *outSlot = dstSlot;
        return 0;
    }
    else if (space == 0)
    {
        if (dstSpace == 3)
        {
            if (dstSlot <= 9 || dstSlot > 0x15)
                return 1;
            if (slot <= 2 || slot > 0x38)
                return 1;
            if (CheckInTrade())
                return 0x13;
            // call 0x0865eed2：arg1=space(0xc(%ebp))、arg2=(short)slot、arg3=dstSlot。
            int ret = change_equip(space, static_cast<short>(slot), dstSlot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            // call 0x0865514c GetParty + 非空调 Reset_party_overlapped_drop_ratio。
            CParty* party = static_cast<CParty*>(GetParty());
            if (party != 0)
                party->Reset_party_overlapped_drop_ratio();
            return 0;
        }
        else if (dstSpace == 0x12)
        {
            if (CheckInTrade())            // 先交易校验（+0x166 处先于区间检查）
                return 0x13;
            if (slot <= 2 || slot > 0x38)
                return 1;
            CExpandEquipslot* expand =
                GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
            // call 0x08499a48：(expand, this, p1, p2, p3, p4)。
            int ret =
                expand->Change_Equip(this, space, slot, dstSpace, dstSlot);
            if (ret != 0)
                return ret;
            *outSlot = dstSlot;
            return 0;
        }
        return 1;
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
    // b28d-40: N82 封顶——帧 0x40 vs 0x58 + 对象槽/callee-saved 分配差，语义闭合。
    // ORIG 0x0866463c。
    BuyItemDeliveryMsg msg;                        // lea -0x20(%ebp) ctor
    int serverGroup = GetServerGroupToPvP();       // T 0x0864a97c
    int characNo = getCurCharacNo();               // W 0x080cbc4e
    unsigned int accId = get_acc_id();             // W 0x080da36e
    msg.setBaseData(2, accId, characNo, serverGroup);  // W 0x086978ec
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
    // b28d-40: N82 封顶——帧 0x1c4 vs 0x1c8 + 槽序平移（bool 槽/中转），语义闭合。
    // ORIG 0x0868745e。
    CLevelRewardTableMgr* mgr = CLevelRewardTableMgr::GetInstance();
    CLevelRewardTable* table = 0;                  // local_28 初值 0
    if (!mgr->IsOn())   // uchar 返回（P9 翻转后）此点 4.4 产 test;sete 两段；
                        // X2 绑 eax 形在本函数实测被 spill 化（mov %eax,-0x24
                        // 续用），且该函数块序 ndiff150 深层非单点可救，回退。
        return;
    table = mgr->GetProperLevelRewardTableR();     // T 0x08687e62
    if (table == 0)
        return;
    if (mgr->IsChallengingJob(job))                // T 0x08687792
        return;
    int jobIdx = table->GetProperJobIndex(job);    // T 0x08687e78
    int levelIdx = table->GetProperLevelIndex(level);  // T 0x086876c8
    if (levelIdx < 0 || jobIdx < 0)
        return;
    int rewardItemNo = table->GetRewardItemNo(jobIdx, levelIdx);  // local_24
    int itemCount = table->GetItemCount(jobIdx, levelIdx);        // local_18
    if (itemCount < 0)
        return;
    char title[21];                                // local_41（21 字节槽）
    strncpy(title, table->GetMailTitle(levelIdx), 0x14);
    char body[256];                                // local_17e
    strncpy(body, table->GetMailString(levelIdx), 0xff);
    Inven_Item item;                               // local_7e（W ctor 0x080cb854）
    int sendCount = 0;                             // local_14
    if (rewardItemNo == 0)
    {
        sendCount = itemCount;                     // 仅金额，道具保持默认构造
    }
    else if (rewardItemNo == 1)
    {
        // +0x177..+0x188 直写字段：+1=2、+2=itemNo、+7=count、+11=1(word)。
        item.m_field1 = 2;
        item.m_addInfo = rewardItemNo;
        item.m_addInfo2 = itemCount;
        item.m_fieldb = 1;
        item.ResetItemAttr();                      // W 0x080cb892
    }
    else
    {
        CItem* script = G_CDataManager()->find_item(rewardItemNo);  // T 0x0835fa32
        if (script == 0)
            return;                                // je +0x25f 直接出口
        script->make_item(item);                   // vtable+8 虚槽（CItem 槽 2）
        item.m_addInfo = rewardItemNo;             // -0x78 ← rewardItemNo
        item.set_add_info(itemCount);              // W 0x080cb884
    }
    ENUM_SERVER_GROUP serverGroup = GetServerGroup();  // W 0x080cbc90
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, item, sendCount, characNo, body,
        static_cast<int>(strlen(body)), 0, serverGroup, 0, 0);
}

// ----------------------------------------------------------------------------
// 4. ORIG T 0x086689ee sz 0x32a。
// Quest 触发物品邮寄：m_mailItems(+0x114) 按 (itemNo,count) 成对遍历；
// itemNo==0 且 count>0 走金额路径（count 入第三参）；否则 find_item 经
// make_item 组道具（count 写 m_addInfo2，第三参置 0）。空列表时寄一封默认
// 道具邮件。标题/正文取 m_mailTitle(+0x10c)/m_mailContents(+0x110)。
void CUser::_postQuestTriggerItem(const Quest* quest)
{
    // b28d-40: N82 封顶——帧 0xdc vs 0xe8 + callee-saved 分配差，语义闭合。ORIG 0x086689ee。
    const std::vector<int>& items = quest->m_mailItems;
    ENUM_SERVER_GROUP serverGroup = GetServerGroup();
    int characNo = getCurCharacNo();
    const char* title = quest->m_mailTitle.c_str();
    const char* body = quest->m_mailContents.c_str();
    int bodyLen = static_cast<int>(quest->m_mailContents.size());
    if (!items.empty())
    {
        // 迭代器成对读取（读 A→++→读 B→处理→++），与 ORIG 循环形态一致。
        for (std::vector<int>::const_iterator it = items.begin(); it != items.end(); )
        {
            int itemNo = *it;
            ++it;
            int count = *it;
            ++it;
            Inven_Item mailItem;                   // local_aa 每对新建
            if (itemNo == 0 && count > 0)
            {
                mailItem.m_addInfo = itemNo;       // -0xa4
                mailItem.m_addInfo2 = count;       // -0x9f
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, mailItem, count, characNo, body, bodyLen, 0,
                    serverGroup, 0, 0);
            }
            else
            {
                CItem* script = G_CDataManager()->find_item(itemNo);
                if (script != 0)
                {
                    script->make_item(mailItem);   // vtable+8
                    mailItem.m_addInfo = itemNo;
                    mailItem.m_addInfo2 = count;
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        title, mailItem, 0, characNo, body, bodyLen, 0,
                        serverGroup, 0, 0);
                }
            }
        }
    }
    else
    {
        Inven_Item mailItem;                       // 空列表单封默认道具邮件
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, mailItem, 0, characNo, body, bodyLen, 0, serverGroup, 0, 0);
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
    // b28d-40: N82 封顶——帧 0x2fc vs 0x310 + 全局重定位/槽平移，语义闭合。
    // ORIG 0x08663cc0。
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (event->IsEventing(0) != true)              // vtable+0x34（槽 13）==1 继续
        return;
    char firstGrow = getCurCharFirstGrowType();    // W 0x08110c94
    int job = get_charac_job();                    // W 0x080fdf20
    int level = get_charac_level();                // W 0x080da2b8
    LevelUpRewardItemTableKey key(level, job, firstGrow);  // W 0x081b8896
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
    char body[256];                                // local_181 起 0x100 清零
    memset(body, 0, sizeof(body));
    strncpy(body, g_scriptStringManager_.findString(
                      4, "game_server_msg_188", (bool*)0),
             0xff);
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        Inven_Item item;                           // local_81（ctor 0x080cb854）
        item.m_addInfo = it->m_field0;             // -0x7b ← data.f0（create_item 前）
        G_CDataManager()->getItemList()->create_item(
            ENUM_ITEM_CREATE_TYPE_0, item, it->m_field4);  // T 0x08511fb8
        ENUM_SERVER_GROUP serverGroup = GetServerGroup();
        int bodyLen = static_cast<int>(strlen(body));
        int characNo = getCurCharacNo();
        const char* mailTitle = g_scriptStringManager_.findString(
            4, "game_server_msg_187", (bool*)0);
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                mailTitle, item, 0, characNo, body, bodyLen, 0, serverGroup,
                0, 0) == 0)
        {
            char history[0x145];                   // local_2c6 缓冲
            memset(history, 0, sizeof(history));
            strncpy(history + 0x11,
                    g_scriptStringManager_.findString(4, "game_server_msg_187",
                                                      (bool*)0),
                    0x14);
            *(reinterpret_cast<int*>(history + 0x39)) = item.m_addInfo;
            *(reinterpret_cast<int*>(history + 0x3d)) = item.m_addInfo2;
            m_historyLog.SendMail(reinterpret_cast<MSG_MAILBOX_SEND*>(history),
                                  0);              // T 0x086847ea
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
    // b28d-40: N82 封顶——槽序平移 + 虚调用链寄存器分配差，语义闭合。ORIG 0x0869115a。
    CEventManager* mgr = GlobalData::s_event_manager;
    CEventBase* event = mgr->GetRepeatEvent(0x68);
    if (event->IsEventing(0))
    {
        // 二次取同一事件（ORIG 再次 GetRepeatEvent(0x68)），判空后调用。
        NewAccountLevelUpEventToJob* levelUpToJob =
            reinterpret_cast<NewAccountLevelUpEventToJob*>(
                mgr->GetRepeatEvent(0x68));
        if (levelUpToJob != 0)
            levelUpToJob->processLevelUpToJob(*this, false);  // T 0x081b82a4
    }
    event = mgr->GetRepeatEvent(0x9e);
    if (event->IsEventing(0))
        ARAD::Singleton<LevelupSupportEventManger>::Get()->UpdateEventInfo(this);
    event = mgr->GetRepeatEvent(0xa3);
    if (event->IsEventing(0))
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
    // b28d-40: P52/P0 单语句链式实参（两实参升格 %esi/%ebx 跨 receiver 调用保活）。
    // 残差 N18a：ORIG 0x0864e0e2 `mov 0x796e0(%eax),%eax; mov %eax,%esi` 两段
    // 中转 vs 4.4 恒折 `mov 0x796e0(%eax),%esi`；字段侧旋钮受 CUser.h 冻结限制。
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
    // b28d-40: P52/P0 单语句链式实参（seed 升格 %ebx 跨 getCurCharacSkillR 保活）。
    // 残差 N18a：ORIG 0x0864e139 `mov 0x796e0(%eax),%eax; mov %eax,%ebx` 两段
    // 中转 vs 4.4 恒折单段；字段侧旋钮受 CUser.h 冻结限制。
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
    // b28d-40: N82 封顶——帧 0x48 vs 0x28 + 迭代器/实参槽分配差，语义闭合。ORIG 0x081b895a。
    for (std::map<LevelUpRewardItemTableKey, LevelUpRewardItemData>::iterator
             it = m_map0.begin();
         it != m_map0.end(); ++it)
    {
        if (it->first.m_grow != key.m_grow)
            continue;
        if (it->first.m_job != key.m_job)
            continue;
        if (it->first.m_lv > key.m_lv)
            continue;
        out.push_back(it->second);
    }
    return !out.empty();
}

// ---- CLevelRewardTable 访问器（偏移与权威布局逐指令核对）----

// ORIG W 0x086976f6：this+8+(jobIdx*0x46+levelIdx+0x44)*4 → m_rewardPrimary 下标。
int CLevelRewardTable::GetRewardItemNo(int jobIdx, int levelIdx)
{
    // 【N18a 标注】同 GetItemCount（ORIG 0x086976f6 双寄存器 lea 形不可翻）。
    // b28c-L18 fresh 探针复证（/tmp/l18p/lea.cpp f_a..f_d）：两操作数序/成员
    // 数组形均恒折 add MEM，封顶（C28L12 代落注记）。
    return m_rewardPrimary[jobIdx * 0x46 + levelIdx];
}

// ORIG W 0x08697712：this+(jobIdx*0x46+levelIdx+0x2bc)*4 → m_rewardSecondary 下标。
int CLevelRewardTable::GetItemCount(int jobIdx, int levelIdx)
{
    // 【N18a 标注】ORIG 0x08697712：双实参先各入寄存器（ecx/edx）且乘积原地
    // 留 ecx 无槽 + lea 合流；f3 具名中间量实测 ndiff 13>8 反劣化（spill/reload
    // +sub/leave），匿名下标折 add mem 为 4.4 恒定选择，封顶（技巧 30 反例族）。
    // b28c-L18 fresh 探针复证（/tmp/l18p/lea.cpp f_a..f_d）：两操作数序/成员
    // 数组形均恒折 add MEM，封顶（C28L12 代落注记）。
    return m_rewardSecondary[jobIdx * 0x46 + levelIdx];
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

// ORIG W 0x08697792 sz 0xc：movzbl (this) 直返全字（无 setne，P9）——uchar 返回。
unsigned char CLevelRewardTableMgr::IsOn()
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
    // b28d-40: N/P75 封顶——形参比较装载序跨版本差（ORIG 先载形参寄存器再比），
    // 语义闭合。ORIG 0x08687d6c。
    if (m_vec8.size() == 1 || (m_vec8.size() != 0 && m_vec8[0] == 0))
        return true;
    if (m_vec8.empty())
        return false;
    for (std::vector<int>::const_iterator it = m_vec8.begin();
         it != m_vec8.end(); ++it)
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
    // b28d-40: N82 封顶——槽 -0x1c vs -0x20 + bool 槽物化 + 全局重定位，语义闭合。
    // ORIG 0x081471c6。
    int level = user->get_charac_level();
    bool pass;
    if (CheckNewStep(level))
    {
        pass = true;
    }
    else
    {
        pass = GlobalData::s_GM_Manager->GetCurrentGmMode(
            user->get_acc_id());                   // T 0x08299276
    }
    if (!pass)
        return;
    unsigned int uid = user->GetUID();             // W 0x080c8c96
    int characNo = user->getCurCharacNo();
    for (unsigned int i = 0; i < user->m_levelupEvents.size(); ++i)
    {
        LevelupSupportEventManger::EventInfo& info = user->m_levelupEvents[i];
        if (info.m_level > level)                  // jg 跳过
            continue;
        if (info.m_state == 2)                     // ==2 视为已完成跳过
            continue;
        info.m_state = 1;                          // movw $0x1, 0x2
        DB_SaveEventLevelupSupport::makeRequest(static_cast<int>(uid),
                                                info.m_level, characNo, 1);
    }
    if (CheckNewStep(level))
        SendCharacState(user, level);
    else
        SendCharacState(user, 0);
}

// ORIG T 0x0814701e sz 0x146（静态）。包头 0x22a：byte 数量 + 每档 (int level,
// byte state) + byte state 参数，finalize(true) 后 Send。
void LevelupSupportEventManger::SendCharacState(CUser* user, int state)
{
    // b28d-40: N82 封顶——帧 0x30 vs 0x20 + 对象槽 -0x1c vs -0x18，语义闭合。
    // ORIG 0x0814701e。
    PacketGuard packet;                            // C1 0x0858dd4c
    packet.clear();
    packet.put_header(0, 0x22a);
    packet.put_byte(static_cast<int>(user->m_levelupEvents.size()));
    for (unsigned int i = 0; i < user->m_levelupEvents.size(); ++i)
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
    // b28d-40: N82 封顶——全局对象绝对寻址 vs 4.4 重定位占位 + 槽序平移，语义闭合。
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
    for (unsigned int i = 0; i < items->size(); ++i)
    {
        // ORIG 用 vector::at(i)（_ZNKSt6vector…2atEj，边界检查）。
        int itemIdx = items->at(i).m_itemIndex;
        CItem* script = G_CDataManager()->find_item(itemIdx);
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
// 入 DB_Q（QUEUE_IDX 2）。
void DB_SaveEventLevelupSupport::makeRequest(int accId, int level,
                                             int characNo, int state)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc25a), true);
    **guard << 0x34f;
    **guard << accId;
    **guard << level;
    **guard << characNo;
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
