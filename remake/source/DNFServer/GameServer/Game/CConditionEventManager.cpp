
#include "CConditionEventManager.h"


#include <string.h>

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "CItemList.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "PacketGuard.h"


namespace
{

// ORIG 0x08334d05/0x08334f9e 两处相同的"入库失败→系统邮件"序列（msg_80/msg_81）。
void insertFailSendMail(CUser* user, Inven_Item& item)
{
    char sender[30];
    strncpy(sender,
            g_scriptStringManager_.findString(4, "game_server_msg_80", 0), 0x1d);
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_81", 0), 0xff);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, 0,
        static_cast<unsigned int>(user->getCurCharacNo()),
        title, static_cast<int>(strlen(title)),
        0, user->GetServerGroup(), false, false);
}

}  // namespace

// ORIG T 0x08334844：空体 C1（push/mov/pop/ret；C1/C2 同址 T，类无虚函数）。
// 调用点：CGameManager::GetConditionEventManager 的 placement new。
// 置于 TU 首个成员定义位，使 _GLOBAL__I_ 键名按 C2Ev（匹配 ORIG 键名，P108）。
CConditionEventManager::CConditionEventManager()
{
}

// ORIG T 0x0833484a：空体 D1（push/mov/pop/ret；D1/D2 同址，类无虚函数）。
CConditionEventManager::~CConditionEventManager()
{
}

// ORIG T 0x08335264
// _ZN22CConditionEventManager30ConditionRewardSameItemCombineERSt6vectorI26stConditionEventRewardInfoSaIS1_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 遍历 infos，按 info.m_field0（物品 id）聚合 m_field4（数量）到 outMap：
// 已存在则累加 second，否则插入 {m_field0, m_field4}。
void CConditionEventManager::ConditionRewardSameItemCombine(
    std::vector<stConditionEventRewardInfo>& infos, std::map<int, int>& outMap)
{
    std::vector<stConditionEventRewardInfo>::iterator it = infos.begin();
    for (; it != infos.end(); ++it)
    {
        std::map<int, int>::iterator mit = outMap.find(it->m_field0);
        if (mit == outMap.end())
        {
            outMap.insert(std::make_pair(it->m_field0, it->m_field4));
        }
        else
        {
            mit->second += it->m_field4;
        }
    }
}

// ORIG T 0x08334850
// _ZN22CConditionEventManager27RequestConditionEventRewardEP5CUserscRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 主奖励链：repeat 事件 0x71 未开启 → 0；curIdx 与 m_conditionEvent.m_field0 不符
// → 1；step<1 / step 超前 / rewardStep != step-1 → 0x13；物品缺失 → 0x11。
// 按 rewardMap 逐条发放：key==1 且 count>=0 → 事件币（EventCoinAdd 日志+保存+发送）；
// 其余 → find_item+create_item，guard（itemIdx==0 或 get_add_info()==0）跳过；
// 非特殊物品直接入库（失败邮件回退），特殊物品按 GetItemType()==0x10 / key==1
// 分支：0x10 直接入库，否则 ProcessSpecialItem（outRestrict!=0 → 0x11）。
// 全部成功 → IncCurConditionEventRewardStep() 并返回 0。
int CConditionEventManager::RequestConditionEventReward(
    CUser* user, short curIdx, char step, std::map<int, int>& rewardMap)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
    {
        return 0;
    }

    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if (curIdx != field0)
    {
        return 1;
    }

            if (step < 1)
        {
            return 0x13;
        }

        unsigned short curStep = user->GetCurConditionEventStep();
        if (curStep < static_cast<unsigned short>(step))
        {
            return 0x13;
        }

        short rewardStep = static_cast<short>(user->GetCurConditionEventRewardStep());
        if (static_cast<int>(rewardStep) != step - 1)
        {
            return 0x13;
        }

    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo(0, infos, curIdx, step);
    ConditionRewardSameItemCombine(infos, rewardMap);

    if (!rewardMap.empty())
    {
        for (std::map<int, int>::iterator it = rewardMap.begin();
             it != rewardMap.end(); ++it)
        {
            if (it->first == 1 && it->second >= 0)
            {
                // ORIG 0x083349ee 区：key==1 → 事件币发放。
                unsigned int coin = user->getCurCharacInvenR()->GetEventCoin();
                user->getCurCharacInvenW()->SetEventCoin(coin + it->second);
                unsigned int newCoin = user->getCurCharacInvenR()->GetEventCoin();
                user->m_historyLog.EventCoinAdd(
                    newCoin, it->second, static_cast<eCoinAddReason>(2));
                user->SaveMoneyCoin();
                user->SendCoin();
                continue;
            }

            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                Inven_Item inv;
                G_CDataManager()->getItemList()->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(0), inv, it->second);

                // ORIG guard（0x08334b07..+0x2df）：itemIdx==0 或 get_add_info()==0 → 不处理。
                bool guardOk = !(it->first == 0 || inv.get_add_info() == 0);
                if (guardOk)
                {
                    int outType = 0;
                    int outValue = 0;
                    bool isSpecial = false;
                    if (GlobalData::s_pCeraShop->IsSpecialItem(it->first, outType, outValue, isSpecial))
                    {
                        // ORIG 0x08334b7f 区：特殊物品 → GetItemType() 检查。
                        int itemType = item->GetItemType();
                        if (itemType == 0x10 || it->first == 1)
                        {
                            // type==0x10 或 itemIdx==1：不做 ProcessSpecialItem；
                            // 仅 type==0x10 直接入库（0x08334d8e 区）。
                            if (itemType == 0x10)
                            {
                                int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                                    inv, static_cast<eItemAddReason>(0x25), true, true);
                                if (slot < 0)
                                {
                                    insertFailSendMail(user, inv);
                                }
                                else if (inv.IsCreatureItemType())
                                {
                                    user->SendUpdateItem(CUser::eSendTarget_1,
                                                         static_cast<ENUM_ITEMSPACE>(7), slot);
                                }
                                else
                                {
                                    user->SendUpdateItem(CUser::eSendTarget_1,
                                                         static_cast<ENUM_ITEMSPACE>(0), slot);
                                }
                            }
                        }
                        else
                        {
                            // ORIG 0x08334bb5 区：ProcessSpecialItem 发放特殊物品。
                            int restrictCode =
                                G_CDataManager()->getItemList()->GetRestrictCode(it->first);
                            long curSec = GlobalData::s_systemTime_.getCurSec();
                            int outRestrict = 0;
                            std::vector<Inven_Item> items;
                            WongWork::CCeraShop::stCeraShopItemParam_t param;
                            bool ok = GlobalData::s_pCeraShop->ProcessSpecialItem(
                                user, it->first, restrictCode, outRestrict, curSec, true,
                                items, param, 0, 0, false, 1);
                            if (ok)
                            {
                                if (outRestrict == 0)
                                {
                                    if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
                                    {
                                        user->SetRestrictedGoods(restrictCode);
                                        GlobalData::s_pCeraShop->SaveFeaturedIdx(
                                            user, restrictCode, curSec, 0);
                                        user->SendRestrictedGoods(true, restrictCode);
                                    }
                                }
                                else
                                {
                                    return 0x11;
                                }
                            }
                        }
                    }
                    else
                    {
                        // ORIG 0x08334f9e 区：非特殊物品 → 直接入库。
                        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                            inv, static_cast<eItemAddReason>(0x25), true, true);
                        if (slot < 0)
                        {
                            insertFailSendMail(user, inv);
                        }
                        else if (inv.IsCreatureItemType())
                        {
                            user->SendUpdateItem(CUser::eSendTarget_1,
                                                 static_cast<ENUM_ITEMSPACE>(7), slot);
                        }
                        else
                        {
                            user->SendUpdateItem(CUser::eSendTarget_1,
                                                 static_cast<ENUM_ITEMSPACE>(0), slot);
                        }
                    }
                }
            }
            else
            {
                return 0x11;
            }
        }
    }

    user->IncCurConditionEventRewardStep();
    return 0;
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

// ORIG T 0x083354f0 _ZN22CConditionEventManager16GetMaxRewardStepEs
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 时返回
// m_conditionEvent.m_map8.size()（+0x4da0 map 的奖励步数上限）；否则 0。
int CConditionEventManager::GetMaxRewardStep(short step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return 0;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)step != field0)
        return 0;
    return G_CDataManager()->m_conditionEvent.m_map8.size();
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

// ORIG T 0x083358bc _ZN22CConditionEventManager16IsDoubleGoldCardEP5CUserc
// CheckErrorCardForStep 不过 → false；普通集合（useQuickPartyAdd=0）按 step 收集
// 奖励信息，IsDoubleGoldCardProcess 为真 → 双倍；否则须为快速组队
// （GetParty 非空且 is_quick_party），再以快速集合（useQuickPartyAdd=1）更新判定。
bool CConditionEventManager::IsDoubleGoldCard(CUser* user, char step)
{
    bool check = CheckErrorCardForStep(user, step);
    bool doubleGoldCard;
    if (!check)
        return false;
    int curIdx = GetCurEventIndex();
    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo((char)0, infos, (short)curIdx, step);
    doubleGoldCard = IsDoubleGoldCardProcess(infos);
    if (!doubleGoldCard && user->GetParty() != 0)
    {
        if (((CParty*)user->GetParty())->is_quick_party())
        {
            std::vector<stConditionEventRewardInfo> quickInfos;
            GetConditionRewardInfo((char)1, quickInfos, (short)curIdx, step);
            doubleGoldCard = IsDoubleGoldCardProcess(quickInfos);
        }
    }
    return doubleGoldCard;
}

// ORIG T 0x08335a3a
// _ZN22CConditionEventManager23IsDoubleGoldCardProcessERSt6vectorI26stConditionEventRewardInfoSaIS1_EE
// 任一 info 的 m_flag9（+0x09 字节）非零 → true。
bool CConditionEventManager::IsDoubleGoldCardProcess(
    std::vector<stConditionEventRewardInfo>& infos)
{
    for (unsigned int i = 0; i < infos.size(); ++i)
    {
        if (infos[i].m_flag9)
            return true;
    }
    return false;
}

// ORIG T 0x0833586c：任一 info.m_flag8（+0x08 字节）非零即免费。
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
