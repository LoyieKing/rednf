// df_game_r 还原 —— NewAccountLevelUpEventToJob（新账号升级转职奖励事件）。
// 全部函数逐指令对照 docs/class_func_reports/NewAccountLevelUpEventToJob/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x081b8170 / D1 0x081b818c / D0 0x081b81bc / StartEvent() 0x081b81da /
//   StartEvent(Word_Param) 0x081b81f6 / EndEvent() 0x081b824c /
//   StartAction() 0x081b8246 / IsEventing 0x081b8816（objdump 转写）/
//   processLevelUpToJob 0x081b82a4 / sendNewAccountLevelUpToJobData 0x081b829e
#include "NewAccountLevelUpEventToJob.h"

#include <string.h>

#include "CUser.h"                    // CUser::getCharacLevelUpGift_AccountOnce
#include "CUserCharacInfo.h"          // getCurCharFirstGrowType/get_charac_job/
                                      // get_charac_level/getCurCharacNo
#include "CDataManager.h"             // G_CDataManager/m_levelUpReward/getItemList
#include "Inven_Item.h"               // Inven_Item（m_addInfo）
#include "CItemList.h"                // create_item/ENUM_ITEM_CREATE_TYPE
#include "CInventory.h"               // OS_API::snprintf
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "DB_EventAccountCharacterLevelUpOnceGift.h"
#include "GameTypes.h"                // cMyTrace/LogManager
#include "GameWorld.h"                // G_GameWorld/IsIntegratedPvPBaseChannel
#include "CEnvironment.h"             // G_CEnvironment/m_serverEnvir.m_serverGroup

// ORIG C1 0x081b8170：CEventBase::CEventBase() + vptr 设置，无自身字段。
NewAccountLevelUpEventToJob::NewAccountLevelUpEventToJob()
{
}

// ORIG D1 0x081b818c：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查。
NewAccountLevelUpEventToJob::~NewAccountLevelUpEventToJob()
{
}

// ORIG 0x081b81da：仅 SetEventFlag(true)，无日志。
void NewAccountLevelUpEventToJob::StartEvent()
{
    SetEventFlag(true);
}

// ORIG 0x081b81f6：虚调 StartEvent()（vptr+8）后
// cMyTrace(__PRETTY_FUNCTION__,0x18,0)("NewAccountLevelUpEvent Event Start!")。
void NewAccountLevelUpEventToJob::StartEvent(Word_Param /*param*/)
{
    // [N7 封顶][b28c-33 复核 22/22] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("NewAccountLevelUpEvent Event Start!");
}

// ORIG 0x081b824c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)。
void NewAccountLevelUpEventToJob::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2b, 0);
    trace("NewAccountLevelUpEvent Event End!");
}

// ORIG 0x081b8246：空函数（push ebp/mov esp,ebp/pop ebp/ret）。
void NewAccountLevelUpEventToJob::StartAction()
{
}

// ORIG 0x081b8816：直调 CEventBase::IsEventing(this, user)（vtable+0x3c 覆盖）。
bool NewAccountLevelUpEventToJob::IsEventing(CUser* user) const
{
    return CEventBase::IsEventing(user);
}

// ORIG 0x081b829e：空函数（push ebp/mov esp,ebp/pop ebp/ret）。
void NewAccountLevelUpEventToJob::sendNewAccountLevelUpToJobData(CUser& /*user*/)
{
}

// ORIG T 0x081b82a4 sz 0x515。
// 新账号升级转职奖励：查奖励表（growup 聚合 / 单项）→ 账号级 once-gift 多重
// 映射去重（同 job 区间内已存在同 level 则中止）→ 插入 (level, job) → DB
// 上报 → 逐条 create_item + 邮寄（标题 game_server_msg_285，正文
// snprintf(game_server_msg_286, level)），失败走 logFormat。成功返回 1，
// 前置失败返回 0。
// b28d-31: N 封顶（N82/P197）——ORIG 0x081b82a4；351 vs 353（帧/槽）。
bool NewAccountLevelUpEventToJob::processLevelUpToJob(CUser& user, bool growup)
{
    // ORIG：全部 getter 在实参位内联求值（P24），不设持久局部 —— key 构造
    // firstGrow→esi(movsbl)、job→ebx 跨调用缓存、level→eax 直传；去重循环
    // 的 ==level 与 makeRequest/snprintf 的 level/job 各使用点重新调用 getter。
    LevelUpRewardItemTableKey key(user.get_charac_level(),
                                  user.get_charac_job(),
                                  user.getCurCharFirstGrowType());
    std::vector<LevelUpRewardItemData> rewards;    // vector C1 0x081b8c00
    if (growup)
    {
        if (!G_CDataManager()->m_levelUpReward.get_data_when_growup(key, rewards))
            return false;                          // ebx=0 出口
    }
    else
    {
        LevelUpRewardItemData data;                // C1 0x081b88bc
        if (!G_CDataManager()->m_levelUpReward.get_data(key, data))
            return false;
        rewards.push_back(data);                   // push_back 0x081b8a52
    }
    // getCharacLevelUpGift_AccountOnce（T 0x08691e5c）返回 &m_levelUpGift
    // （+0x8ec04，+0x00 即 multimap<int,int>）；empty/lower_bound/upper_bound/
    // insert 均以该指针为 this。迭代器经默认构造 + 赋值（临时中转拷贝，
    // P28），upper_bound 结果缓存，循环体内 it!=end() 防御检查。
    std::multimap<int, int>& gifts =
        *static_cast<std::multimap<int, int>*>(
            user.getCharacLevelUpGift_AccountOnce());
    if (!gifts.empty())
    {
        std::multimap<int, int>::iterator it;
        it = gifts.lower_bound(user.get_charac_job());
        std::multimap<int, int>::iterator upper =
            gifts.upper_bound(user.get_charac_job());
        while (it != upper)
        {
            if (it != gifts.end() && it->second == user.get_charac_level())
                return false;                      // 该等级已领取 → 中止
            ++it;
        }
    }
    gifts.insert(std::make_pair(user.get_charac_job(),
                                user.get_charac_level()));
    DB_EventAccountCharacterLevelUpOnceGift::makeRequest(
        user.get_acc_id(), static_cast<short>(user.GetServerGroupToPvP()),
        user.getCurCharacNo(), static_cast<short>(user.get_charac_job()),
        static_cast<short>(user.get_charac_level()));
    char body[256] = { '\0' };                     // -0x1b5 起 0x100 清零（P4 内联 memset）
    OS_API::snprintf(body, 0xff,
                     g_scriptStringManager_.findString(4, "game_server_msg_286",
                                                       (bool*)0),
                     user.get_charac_level());
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        Inven_Item item;
        item.m_addInfo = it->m_field0;             // -0xb3 ← data.f0
        G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0,
                                                  item, it->m_field4);
        // findString("game_server_msg_285") 在循环体内实参位（ORIG 每次迭代调）。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_285",
                                                  (bool*)0),
                item, 0, user.getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()), 0,
                0) != 0)
        {
            LogManager::logFormat(
                1, "NewAccountLevelUpEventToJob.cpp",
                __PRETTY_FUNCTION__,
                0x7f, "NewAccountLevelUpEventToJob Event Start!");
        }
    }
    return true;                                   // ebx=1
}
