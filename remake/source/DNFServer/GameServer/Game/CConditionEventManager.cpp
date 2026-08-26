// df_game_r Game/ CConditionEventManager 实现（批3：从 GameStubs.cpp 迁移）。
#include "CConditionEventManager.h"

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "PacketGuard.h"

CConditionEventManager::CConditionEventManager()
{
}

// ORIG 0x08335754
// _ZN22CConditionEventManager16GetCurEventIndexEv
// 若当前重复事件 0x71 处于进行中（IsEventing(0) 真）返回 CDataManager
// m_conditionEvent.m_field0（+0x4d98，当前条件事件索引）；否则返回 0。
int CConditionEventManager::GetCurEventIndex()
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return 0;
    return G_CDataManager()->m_conditionEvent.m_field0;
}

// ORIG T 0x083356a4 _ZN22CConditionEventManager21_getRequiredParameterEs
// sret pair<int,int>：查 m_conditionEvent.m_map8（G_CDataManager()+0x4da0，
// map<int,pair<int,int>>）键=step；命中 → {second.first, second.second}；
// 未命中 → {-1,-1}。ret $0x4（sret 指针随 ret 清栈）。
std::pair<int, int> CConditionEventManager::_getRequiredParameter(short step)
{
    std::pair<int, int> result;
    int key = (int)step;
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_conditionEvent.m_map8.find(key);
    std::map<int, std::pair<int, int> >::iterator endIt =
        G_CDataManager()->m_conditionEvent.m_map8.end();
    if (it != endIt)
    {
        result.first = it->second.first;
        result.second = it->second.second;
        return result;
    }
    result.first = -1;
    result.second = -1;
    return result;
}

// ORIG T 0x083355e6 _ZN22CConditionEventManager31_processProperDungeonClearEventEP5CUsers
// 取当前条件事件步 curStep；_getRequiredParameter(step+1) 得 (required,-)；
// required != -1 且 required <= GetProperDungeonClearCount() → 步进 + 发送条件事件信息；
// 无条件发送清剿计数。ORIG 中 PacketGuard 局部对象构造后未使用（照抄保留）。
void CConditionEventManager::_processProperDungeonClearEvent(CUser* user, short step)
{
    int curStep = (short)user->GetCurConditionEventStep();
    std::pair<int, int> required = _getRequiredParameter((short)(curStep + 1));
    int clearCount = (short)user->GetProperDungeonClearCount();
    PacketGuard packet;
    if (required.first != -1 && required.first <= clearCount)
    {
        user->IncCurConditionEventStep();
        user->SendConditionEventInfo();
    }
    user->SendProperDungeonClearCount();
}

// ORIG T 0x08335a8a _ZN22CConditionEventManager21CheckErrorCardForStepEP5CUserc
// user 非空、IsProperDungeonUser()、step>=0 三关；_getRequiredParameter(step) 得
// required：==-1 或 < GetProperDungeonClearCount() → false；否则 true。
bool CConditionEventManager::CheckErrorCardForStep(CUser* user, char step)
{
    if (user == NULL)
        return false;
    if (!user->IsProperDungeonUser())
        return false;
    if (step < 0)
        return false;
    std::pair<int, int> required = _getRequiredParameter(step);
    int clearCount = (short)user->GetProperDungeonClearCount();
    if (required.first == -1)
        return false;
    if (required.first < (int)clearCount)
        return false;
    return true;
}

// ORIG T 0x083353a0
// _ZN22CConditionEventManager22GetConditionRewardInfoEcRSt6vectorI26stConditionEventRewardInfoSaIS1_EEsc
// 事件 0x71 未进行中直接返回；(short)curIdx 与 m_conditionEvent.m_field0 不符
// 直接返回；out.clear() 后按 useQuickPartyAdd 选 m_multiMap38(+0x4dd0)/m_multiMap20
// (+0x4db8)，取 equal_range(step) 全部 value push_back 进 out。
void CConditionEventManager::GetConditionRewardInfo(
    char useQuickPartyAdd, std::vector<stConditionEventRewardInfo>& out,
    short curIdx, char step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)curIdx != field0)
        return;
    out.clear();
    std::multimap<int, stConditionEventRewardInfo>* mm;
    if (!useQuickPartyAdd)
        mm = &G_CDataManager()->m_conditionEvent.m_multiMap20;
    else
        mm = &G_CDataManager()->m_conditionEvent.m_multiMap38;
    std::multimap<int, stConditionEventRewardInfo>::iterator it =
        mm->lower_bound((int)step);
    std::multimap<int, stConditionEventRewardInfo>::iterator endIt =
        mm->upper_bound((int)step);
    for (; it != endIt; ++it)
    {
        out.push_back(it->second);
    }
}

// ORIG T 0x0833586c
// _ZN22CConditionEventManager21IsFreeGoldCardProcessERSt6vectorI26stConditionEventRewardInfoSaIS1_EE
// 任一 info 的 m_flag8（+0x08 字节）非零 → true。
bool CConditionEventManager::IsFreeGoldCardProcess(
    std::vector<stConditionEventRewardInfo>& infos)
{
    for (unsigned int i = 0; i < infos.size(); ++i)
    {
        if (infos[i].m_flag8)
            return true;
    }
    return false;
}

// ORIG T 0x0833579e _ZN22CConditionEventManager14IsFreeGoldCardEP5CUserc
// CheckErrorCardForStep 不过 → false；取当前条件事件索引，按 step 收集奖励信息，
// IsFreeGoldCardProcess 判定免费金卡。
bool CConditionEventManager::IsFreeGoldCard(CUser* user, char step)
{
    bool check = CheckErrorCardForStep(user, step);
    if (!check)
        return false;
    short curIdx = (short)GetCurEventIndex();
    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo((char)0, infos, curIdx, step);
    bool freeGoldCard = IsFreeGoldCardProcess(infos);
    return freeGoldCard;
}

// ORIG T 0x08335566 _ZN22CConditionEventManager18ProcessCheckStepUpEP5CUsers
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 == 2 时推进晋级流程。
void CConditionEventManager::ProcessCheckStepUp(CUser* user, short step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)step != field0)
        return;
    if ((int)step == 2)
        _processProperDungeonClearEvent(user, step);
}
