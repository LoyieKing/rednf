// ============================================================================
// CUserStatA.cpp —— CUser 等级/属性域三符号独立 TU（Prod_CUserA 批次）。
// 仅承载三个真实 ORIG 符号（已 grep 全工程确认无既有定义，2026-08-24 广播）：
//   CUser::_update_charac_stat(bool)   ORIG T 0x086646c8
//                                      _ZN5CUser19_update_charac_statEb
//   CUser::_onLevelUp()                ORIG T 0x0866311a
//                                      _ZN5CUser10_onLevelUpEv
//   CUser::UpdateTutorialSkipable()    ORIG W 0x08697318
//                                      _ZN5CUser22UpdateTutorialSkipableEv
// 依据 docs/class_func_reports/CUser/{_update_charac_stat,_onLevelUp}.md 全文；
// UpdateTutorialSkipable 无报告，按 objdump 逐指令转写（见函数注释）。
// 被调 _processLevelReward/_postQuestTriggerItem/processLevelUpEventReward/
// processLevelUpEvent、DB_Insert{UnlimitSupport,ArchieveEvent}Log::makeRequest、
// EventClassify::CEventScriptMng::process_level_up_reward、CFatigueBatteryHandle::
// ActiveFatigueBattery_EndLevel 为"声明先行"依赖，定义由其它批次还原。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"          // G_CDataManager/m_characList/find_quest/find_item/get_event_script_mng
#include "CCharacter.h"            // CCharacter 布局（0x7dc 步长）/get_growtype_stat/_Additioal_info
#include "Quest.h"                 // Quest::m_mailFlag/m_levelMin、stSelectQuestParam(CUser*)
#include "CInventory.h"            // GetInvenRef/GetInvenData/insertItemIntoInventory/update_item
#include "Inven_Item.h"            // m_addInfo/m_field1/set_add_info/ResetItemAttr
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"     // CEventBase/CEventManager::GetRepeatEvent
#include "CFatigueBatteryHandle.h"
#include "CSystemTime.h"
#include "CEventScriptMng.h"
#include <string.h>
#include <list>

class DB_InsertUnlimitSupportLog
{
public:
    // ORIG 0x0842ebd0：_ZN26DB_InsertUnlimitSupportLog11makeRequestEjjPKc（返回 void，
    // 见 DB_InsertUnlimitSupportLog/makeRequest.md 反编译）。类声明与 DB_U3.cpp/
    // CMailBoxHelper.cpp 的 TU 局部前置声明模式一致。
    static void makeRequest(unsigned int uid, unsigned int characNo,
                            const char* name);
};

class DB_InsertArchieveEventLog
{
public:
    // ORIG 0x0842e856：
    // _ZN25DB_InsertArchieveEventLog11makeRequestEjjN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE
    // （返回 void，见 DB_InsertArchieveEventLog/makeRequest.md 反编译）。
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type);
};

// ============================================================================
// CUser::_update_charac_stat(bool) @ 0x086646c8
// 报告：docs/class_func_reports/CUser/_update_charac_stat.md（汇编+C 全文）。
// 语义：取当前角色 (firstGrow,secondGrow) 对应职业成长属性块（CCharacList 以
// 0x7dc 步长的 CCharacter 数组，基址 = CDataManager::m_characList），将其
// add/subtract 到当前角色 AddInfo（写侧），add=false 时回退。
// ============================================================================
void CUser::_update_charac_stat(bool add)
{
    char secondGrow = getCurCharSecondGrowType();          // +0x15 call getCurCharSecondGrowType
    char firstGrow  = getCurCharFirstGrowType();           // +0x23 call getCurCharFirstGrowType

    CCharacter* characTable =                              // +0x2b..+0x33 *(G_CDataManager()+0x14)
        reinterpret_cast<CCharacter*>(G_CDataManager()->m_characList);
    int job = get_charac_job();                            // +0x39 call get_charac_job

    const _Additioal_info* stat =                          // +0x52 call get_growtype_stat(job*0x7dc 行)
        static_cast<const _Additioal_info*>(
            characTable[job].get_growtype_stat(firstGrow, secondGrow));

    // ORIG：getCurCharacAddInfoW 在 add/subtract 两分支各自重调（禁缓存，P183/P48）。
    if (add)                                               // +0x5a cmpb param
        static_cast<_Additioal_info*>(getCurCharacAddInfoW())
            ->add(*stat);                                  // +0x75 _Additioal_info::add 0x08146300
    else
        static_cast<_Additioal_info*>(getCurCharacAddInfoW())
            ->subtract(*stat);                             // +0x91 _Additioal_info::subtract 0x08693d04
}

// ============================================================================
// CUser::_onLevelUp() @ 0x0866311a
// 报告：docs/class_func_reports/CUser/_onLevelUp.md（汇编+C 全文）。
// 升级尾处理：限时日志 → 等级奖励 → 达成事件日志 → 公会服邮件金币 → 邮件任务
// 触发 → 会员限时时邮件道具 → NPC 礼物/升级事件 → 85 级疲劳电池 → 20 级补发
// 4183 号物品 → 事件脚本升级奖励。
// ============================================================================
void CUser::_onLevelUp()
{
    // ---- ① 15 级且角色创建时间在活动窗内：记 unlimit 支援日志（+0x0c..+0x81）----
    if (get_charac_level() == 0xf
        && getCurCharacCreateTime() > 0x47bc94cf           // +0x27 cmp/jle（有符号）
        && GlobalData::s_systemTime_.getCurSec() < 0x47d3fb71)  // +0x35 cmp $0x47d3fb70/jg
    {
        DB_InsertUnlimitSupportLog::makeRequest(get_acc_id(), (unsigned)getCurCharacNo(),
                                                getCurCharacName());
    }

    // ---- ② 等级奖励（+0x86..+0xc3）----
    _processLevelReward((unsigned)get_charac_level(),      // _ZN5CUser19_processLevelRewardEjjc
                        (unsigned)getCurCharacNo(),
                        (char)get_charac_job());

    // ---- ③ 重复事件 0x15 活跃时：18/48 级达成事件日志（+0xc8..+0x176）----
    // ORIG 经 vptr+0x34（slot13=IsEventing(CUser*)）判活，实参传 0；无判空（忠实转写）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x15)->IsEventing((CUser*)0))
    {
        if (get_charac_level() == 0x12)
            DB_InsertArchieveEventLog::makeRequest(get_acc_id(), (unsigned)getCurCharacNo(),
                                                   SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE_1);
        if (get_charac_level() == 0x30)
            DB_InsertArchieveEventLog::makeRequest(get_acc_id(), (unsigned)getCurCharacNo(),
                                                   SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE_2);
    }

    // ---- ④ 服务器组 10 活动窗内 30/40 级：系统邮件发放金币（+0x17b..+0x3ce）----
    // G_CEnvironment()+0x378 == m_serverEnvir.m_serverGroup == 10；
    // 时间上界 0x478e7051（cmp $0x478e7050/ja → <=0x478e7050 通过）。
    if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 10
        && GlobalData::s_systemTime_.getCurSec() < 0x478e7051)
    {
        if (get_charac_level() == 0x1e || get_charac_level() == 0x28)
        {
            Inven_Item item;                                   // +0x1ea Inven_Item::Inven_Item（两分支共享）
            char desc[256];                                    // local_26c（strncpy 上限 0xff）

            if (get_charac_level() == 0x1e)                    // +0x1fd 分支 A
            {
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_11", (bool*)0),
                        0xff);                                 // +0x227/+0x241
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_09", (bool*)0),
                    item, 500000, (unsigned)getCurCharacNo(),  // +0x2c2 movl $0x7a120
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
            else                                               // +0x2e1 分支 B（level==0x28）
            {
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_12", (bool*)0),
                        0xff);                                 // +0x319/+0x333
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_09", (bool*)0),
                    item, 1000000, (unsigned)getCurCharacNo(), // +0x3b4 movl $0xf4240
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
        }
    }

    // ---- ⑤ 可邮寄任务触发（+0x3ce..+0x4d5）：list<int> + stSelectQuestParam ----
    std::list<int> mailQuests;
    {
        stSelectQuestParam selectParam(this);                  // +0x3e6 stSelectQuestParam(CUser*)
        if (getCurCharacQuestR()->get_mail_quest_info(mailQuests, selectParam))  // +0x407/test %al
        {
            for (std::list<int>::iterator it = mailQuests.begin();
                 it != mailQuests.end(); ++it)                 // +0x414..+0x4cf 迭代
            {
                Quest* quest = G_CDataManager()->find_quest(*it);   // +0x44a find_quest
                if (quest != 0                                  // +0x456
                    && quest->m_mailFlag == 0                   // +0x462 movzbl 0x108/test
                    && get_charac_level() == quest->m_levelMin) // +0x479 cmp 0x20(m_levelMin)
                {
                    _postQuestTriggerItem(quest);               // +0x48d
                }
            }
        }
    }                                                          // +0xb95 ~list

    // ---- ⑥ 会员（premium type 0x10）限时内 30/40 级：按背包槽位补发道具邮件（+0x4d5..+0x7a6）----
    // 时间上界 0x46e1bbd0（cmp/ja → <=0x46e1bbd0 通过）。
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x10)
        && GlobalData::s_systemTime_.getCurSec() <= 0x46e1bbd0)
    {
        const Inven_Item* ref =                                // +0x51e GetInvenRef(INVEN 0, slot 0xb)
            getCurCharacInvenR()->GetInvenRef(0, 0xb);
        if ((unsigned)(ref->m_addInfo - 0x65d6) <= 4)          // +0x536 sub/cmp $0x4/ja（无符号区间）
        {
            if (get_charac_level() == 0x1e)                    // +0x558 分支 A
            {
                char desc[256];                                // local_26c
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_05", (bool*)0),
                        0xff);                                 // +0x57f/+0x599
                Inven_Item item;                               // +0x5a7 Inven_Item::Inven_Item（&local_f2）
                item.m_addInfo = 0xf;                          // +0x5b6 movl $0xf（item+2）
                item.m_field1 = 2;                             // +0x5bd movb $0x2（item+1，ITEM_TYPE）
                item.m_addInfo2 = 5;                           // +0x5c7 movl $0x5（item+7 堆叠数）
                item.m_fieldb = 0;                             // +0x5d0 movw $0x0（item+0xb 耐久）
                item.ResetItemAttr();                          // +0x5d9

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_08", (bool*)0),
                    item, 0, (unsigned)getCurCharacNo(),
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
            else if (get_charac_level() == 0x28)               // +0x679 分支 B
            {
                char desc[256];
                strncpy(desc, g_scriptStringManager_.findString(4, "game_server_msg_06", (bool*)0),
                        0xff);                                 // +0x6b1/+0x6cb
                Inven_Item item;                               // +0x6d9（&local_12f）
                item.m_addInfo = 0x24;                         // +0x6de movl $0x24
                item.m_field1 = 2;                             // +0x6e8 movb $0x2
                item.m_addInfo2 = 5;                           // +0x6ef movl $0x5
                item.m_fieldb = 0;                             // +0x6f9 movw $0x0
                item.ResetItemAttr();                          // +0x70b

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    /*pSender*/ g_scriptStringManager_.findString(4, "game_server_msg_08", (bool*)0),
                    item, 0, (unsigned)getCurCharacNo(),
                    /*title*/ desc, strlen(desc), 0, GetServerGroup(), false, false);
            }
        }
    }

    // ---- ⑦ NPC 升级礼物 / 升级事件奖励 / 升级事件（+0x7a6..+0x7cf）----
    processNPCGiftOnLevelUp();                                 // 0x0866407a
    processLevelUpEventReward(false);                          // 0x08663cc0（实参 false）
    processLevelUpEvent();                                     // 0x0869115a

    // ---- ⑧ 85 级：疲劳电池 EndLevel 结算与日志（+0x7cf..+0x87b）----
    if (get_charac_level() == 0x55)
    {
        int descFatigue =                                      // cwtl 符号扩展 short→int
            (short)GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery_EndLevel(this);
        if (descFatigue != 0)
        {
            DecFatigueBatteryCharging((short)descFatigue);     // +0x816 DecFatigueBatteryCharging
            short charging =
                (short)GetFatigueBatteryCharging();            // +0x821 movswl（ORIG 按 signed 读）
            cMyTrace trace(__PRETTY_FUNCTION__, 0x4290, 0);   // +0x854 __PRETTY_FUNCTION__/行号
            trace("FATIGUE BATTERY ACTIVE END_LEVEL: charno(%d) , descFatigueAmount(%d), cur charging(%d)",
                  (int)getCurCharacNo(), descFatigue, (int)charging); // +0x876
        }
    }

    // ---- ⑨ 20 级：补发 4183(0x1057) 号物品至堆叠上限 3（+0x881..+0xb4e）----
    if (get_charac_level() == 0x14)
    {
        CItem* script = G_CDataManager()->find_item(0x1057);   // +0x8a4 find_item
        if (script != 0)
        {
            Inven_Item item;                                   // +0x8bf Inven_Item::Inven_Item
            int slot = getCurCharacInvenR()->GetInvenData(0x1057, item);  // +0x8e4 GetInvenData
            if (slot < 0)                                      // +0x8f0 jns
            {
                script->make_item(item);                       // +0x910 vtbl+0x08 make_item(Inven_Item&)
                item.m_addInfo = 0x1057;                       // +0x91c movl（item+2）
                item.m_addInfo2 = 3;                           // +0x912 movl $0x3（item+7 堆叠数）

                slot = getCurCharacInvenW()->insertItemIntoInventory(
                    item, eItemAddReason_0xd, true, false);    // +0x9ed（reason/bLog/b=false）
            }
            else if (item.m_addInfo2 <= 2)                     // +0xa00 cmp $0x2/jg（当前堆叠<3）
            {
                int curCount = item.get_add_info();            // +0xa17 调用取值（ItemAdd 实参）
                m_historyLog.ItemAdd(INVEN_TYPE_EQUIP, 3 - curCount, curCount,
                                     item, eItemAddReason_0xd);// +0xa54（INVEN_TYPE 1/reason 0xd）
                item.m_addInfo2 = 3;                           // +0xa63 movl $0x3
                getCurCharacInvenW()->update_item(INVEN_TYPE_EQUIP, slot, item);  // +0xb21
            }
            if (slot >= 0)                                     // +0xb2a js
                SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);       // +0xb49
        }
    }

    // ---- ⑩ 事件脚本管理器升级奖励（+0xb53..+0xb72）----
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->process_level_up_reward(this, false);                // 0x0810bf56（实参 false）
}

// ============================================================================
// CUser::UpdateTutorialSkipable() @ 0x08697318 —— 无报告，objdump 逐指令转写：
//   8697324: call 819a870 <CUserCharacInfo::enableSaveCharacStat()>  ; this+0x1 = 1
//   8697337: call 84ecae4 <CUser::SetTutorialSkipable(unsigned char)>; 实参 1
//   （SetTutorialSkipable 写 CUser+0x8e08a 字节；GetTutorialSkipable 同址读取。）
// ============================================================================
void CUser::UpdateTutorialSkipable()
{
    enableSaveCharacStat();
    SetTutorialSkipable(1);
}
