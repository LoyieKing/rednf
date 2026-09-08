// df_game_r Game/ ARAD::Arad_EventPeriodDataManager 实现
//（批3：从 GameStubs.cpp 迁移；批4：补 Arad_InterfaceGameScript 基类生命周期）。
//
// ORIG ctor 0x81953fa：Arad_InterfaceGameScript("Arad_EventPeriodDataManager") 基类 +
// vector<SEventPeriodData>(+0x0c) / vector(+0x18) / Mutex(+0x24) / list(+0x3c)，+0x08=0。
// Load() 0x8195c9a：清理 + 装载事件周期脚本（loadScriptData）。
// isApplied 0x81956b0：在 +0x0c vector 按事件码找周期项，判定 [m_beginTime, m_endTime)。
#include "Arad_EventPeriodDataManager.h"

#include <algorithm>

#include "CUser.h"       // get_acc_id / get_charac_no
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM 权威声明
#include "GlobalData.h"  // s_pGMAccounts_
#include "LogManager.h"
#include "MoreTypes.h"   // arad::SigLoadRewardEventItem 完整定义

namespace ARAD
{

Arad_EventPeriodDataManager::Arad_EventPeriodDataManager()
    : Arad_InterfaceGameScript("Arad_EventPeriodDataManager")
{
    m_field8 = 0;
}

Arad_EventPeriodDataManager::~Arad_EventPeriodDataManager()
{
    clearScriptData();    // ORIG 0x0819554c：D1 写 vptr 后调 clearScriptData()
}

bool Arad_EventPeriodDataManager::loadScriptData()
{
    // ORIG 0x08195650：直接调 importAradEventScript(m_eventPeriodList,
    // m_rewardPeriodList)（+0xc/+0x18 两 vector），无 return（eax 残留）。
    ARAD::SCRIPT::importAradEventScript(m_eventPeriodList, m_rewardPeriodList);
}

void Arad_EventPeriodDataManager::clearScriptData()
{
    // ORIG 0x8195670：仅清空 +0x0c 的 vector<SEventPeriodData>（奖励容器不清）。
    m_eventPeriodList.clear();
}

// [b28d-28] DIFF | ORIG 0x081956b0 | N86/P28：find_if sret 临时槽序（ORIG
// -0x1c/-0x18 vs 4.4 -0x18/-0x14）+ 谓词实参装载序；语义等价。
bool Arad_EventPeriodDataManager::isApplied(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time)
{
    // P167：ORIG 用 SearchEventPeriod 谓词 + find_if（非 const iterator）
    SearchEventPeriod pred(eventCode);
    std::vector<ARAD::SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    if (!(it != m_eventPeriodList.end()))
        return false;
    return (it->m_beginTime <= time) && (time < it->m_endTime);
}

bool Arad_EventPeriodDataManager::isRunning(ENUM_REPEAT_EVENT_CODE eventCode)
{
    // ORIG isRunning@0x08195686：以 s_systemTime_（0x941f714）的当前 tick 作为
    // time 转发给 isApplied（getCurTickCount@0x081458ac 返回 m_curTickCount）。
    return isApplied(eventCode, GlobalData::s_systemTime_.getCurTickCount());
}

// ORIG 0x08195994：遍历 SigLoadRewardEventItem 条目（count=+0x04 short），
// 每条以 {事件码@+0x06+4i, accId, 角色号@+0x2e+4i} 构造 EventRewardItemArad；
// 未奖励过则 addUserRewardData 记录，否则 logFormat 打印 already reward。
void Arad_EventPeriodDataManager::addUserRewardData(
    CUser* user, arad::SigLoadRewardEventItem& sig)
{
    for (int i = 0; i < (int)sig.m_count; ++i)
    {
        EventRewardItemArad item(sig.m_eventCodes[i], user->get_acc_id(),
                                 sig.m_characNos[i]);
        if (isRewarded(item))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_EventPeriodDataManager.cpp",
                "void ARAD::Arad_EventPeriodDataManager::addUserRewardData(CUser*, "
                "arad::SigLoadRewardEventItem&)",
                0xe3,
                "[FnReward] already reward item. (mid:%u, charac:%u, event:%d)",
                user->get_acc_id(), user->get_charac_no(-1), sig.m_eventCodes[i]);
        }
        else
        {
            addUserRewardData(user, item);
        }
    }
}

// ORIG 0x08195aa4：Guard<Mutex>(this+0x24) 后 push_back 到 +0x3c list。
void Arad_EventPeriodDataManager::addUserRewardData(
    CUser* user, EventRewardItemArad& item)
{
    (void)user;
    Guard<Mutex> guard(&m_mutex);
    m_rewardItemList.push_back(item);
}

// ORIG 0x081958b6：Guard<Mutex> 后 std::find（operator== 三字段比较），命中即 true。
// [b28d-28] DIFF | ORIG 0x081958b6 | N86/P28：std::find 迭代器槽序 + EH 块序
// （ORIG 两段 return 直落 vs 4.4 jmp 岛）；语义等价。
bool Arad_EventPeriodDataManager::isRewarded(EventRewardItemArad item)
{
    Guard<Mutex> guard(&m_mutex);
    return std::find(m_rewardItemList.begin(), m_rewardItemList.end(), item) !=
           m_rewardItemList.end();
}

// ORIG 0x08195794：SearchEventPeriod 谓词 find_if +0x0c vector；
// 命中后起点 = GM ? m_field8 * (-86400) + 0x5005D250 : 0x5005D250（32 位回绕，
// -0x15180 = -86400 秒/天）；begin <= time < 条目 m_endTime(+0x08) 返回 true。
// 注意：ORIG 此函数不加锁。
// [b28d-28] DIFF | ORIG 0x08195794 | N86/P28：同 isApplied（find_if 槽序 +
// 谓词实参序）+ isGM 分支块序；语义等价。
bool Arad_EventPeriodDataManager::checkCreateTime(
    ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time, unsigned int gmId)
{
    SearchEventPeriod pred(eventCode);
    std::vector<SCRIPT::SEventPeriodData>::iterator it =
        std::find_if(m_eventPeriodList.begin(), m_eventPeriodList.end(), pred);
    // P167：ORIG 生成 operator!=
    if (!(it != m_eventPeriodList.end()))
    {
        return false;
    }

    bool isGM = GlobalData::s_pGMAccounts_->isGM(gmId);
    unsigned int beginTime = 0x5005D250u;
    if (isGM)
    {
        beginTime += (unsigned int)(m_field8 * -86400);
    }
    return (beginTime <= time) && (time < it->m_endTime);
}

// ORIG 0x08195b06：Guard<Mutex> 后删除 +0x3c list 中 +0x04 == get_acc_id(user) 的记录。
// [b28d-28] DIFF | ORIG 0x08195b06 | N86/P28：list erase 迭代器槽序
// （ORIG -0x18/-0x1c vs 4.4 -0x1c/-0x18）+ 比较方向（P220）；语义等价。
void Arad_EventPeriodDataManager::delUserRewardData(CUser* user)
{
    Guard<Mutex> guard(&m_mutex);
    unsigned int accId = user->get_acc_id();
    std::list<EventRewardItemArad>::iterator it = m_rewardItemList.begin();
    while (it != m_rewardItemList.end())
    {
        if (it->m_accId == accId)
        {
            it = m_rewardItemList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// ORIG 0x0819589e：param >= 0 时写入 +0x08。
// [b28 BM11] P223：ORIG js→空块 nop 岛 + body 尾 jmp；4.4 对 if{} 恒
// jump-thread（少 jmp+nop），复现形=「负条件 goto 尾空标签」。
void Arad_EventPeriodDataManager::setCreatePeriod_GM(int days)
{
    if (days < 0) goto L_skip;
    m_field8 = days;
L_skip:
    ;
}

}  // namespace ARAD
