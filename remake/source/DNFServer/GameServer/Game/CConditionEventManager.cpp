// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CConditionEventManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CConditionEventManager.h"
// ============================================================================
// b28d-52 判词表（本 TU）：
//   已修：_getRequiredParameter 0x083356a4（P236/P293 内联实参翻槽序）、
//     IsFreeGoldCard 0x0833579e（P0 单变量复用）、ProcessCheckStepUp 0x08335566
//     （P286 单 case switch）⇒ 均转 AE。
//   剩余 DIFF：
//   GetConditionRewardInfo 0x083353a0 —— N86（4.1 iterator 临时槽+拷贝两段；
//     4.4 copy-elision 直构 ⇒ 槽序 -0x28/-0x24 不可复现）
//   RequestConditionEventReward 0x08334850 —— 已修：删发明 helper insertFailSendMail
//     （ORIG nm 0 命中）改两处内联 + inv.m_addInfo(+0x02) 缓存 itemIdx + guard 内联短路
//     + GetItemType 双调/否定守卫块序 + slot 两段式(-1 初始化) + bool ok 内联 ⇒
//     gap 76→4、帧 0x25c→0x24c 全对齐、p167 双向差集仅剩 C1/C2·D1/D2 同址别名（P93）。
//     残余 4 条 = ORIG `mov slot,%eax; shr $0x1f,%eax; test %al,%al; je` vs 4.4
//     `cmpl $0x0,slot; jns`（2 处 ×2 条；`slot<0` 与 `(unsigned)slot>>31` 两形实测
//     4.4 均折 cmpl/jns ⇒ 跨版本 lowering，记 N）。
// ============================================================================


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


// ORIG T 0x08334844：空体 C1（push/mov/pop/ret；C1/C2 同址 T，类无虚函数）。
// 调用点：CGameManager::GetConditionEventManager 的 placement new。
// 置于 TU 首个成员定义位，使 _GLOBAL__I_ 键名按 C2Ev（匹配 ORIG 键名，P108）。
#include "CConditionEventManager/CConditionEventManager.inc"
{
}

// ORIG T 0x0833484a：空体 D1（push/mov/pop/ret；D1/D2 同址，类无虚函数）。
#include "CConditionEventManager/dtor_CConditionEventManager.inc"
{
}

// ORIG T 0x08335264
// _ZN22CConditionEventManager30ConditionRewardSameItemCombineERSt6vectorI26stConditionEventRewardInfoSaIS1_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 遍历 infos，按 info.m_field0（物品 id）聚合 m_field4（数量）到 outMap：
// 已存在则累加 second，否则插入 {m_field0, m_field4}。
#include "CConditionEventManager/ConditionRewardSameItemCombine.inc"
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
#include "CConditionEventManager/RequestConditionEventReward.inc"
    CUser* user, short curIdx, char step, std::map<int, int>& rewardMap)
{
    // [L6S2b 收尾 · N 封顶] ORIG 614 / our 610（负 gap +4）、ndiff 12。归一化逐行比对
    //（/tmp/L6S2_probe/normdiff.py：去 call/jmp 目标与重定位占位后）真实残差**只有**
    // 两处：ORIG `mov -0x20(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je`
    //（0x08334e64 / 0x08335074，insertItemIntoInventory 返回值 `< 0` 判定）vs 4.4
    // `cmpl $0x0,-0x20(%ebp); jns`——2 处 × 2 条即 gap +4 的全部来源。
    // 探针 /tmp/L6S2_probe/pr_sign.cpp f1..f5（`r<0` / `(unsigned)r>>31` / 具名 unsigned
    // +`>>31` / `r&0x80000000` / `r>>31`）在 4.4.7 下全部折叠为 `cmpl $0; jns`（f3 仅多
    // 一条 mov+test）⇒ 4.1 独有的 shr 符号位提取 lowering，源码层不可复现：按 §6.4
    //「负 gap 命中 ORIG 独有段即判 N」封顶。
    // 其余差异全为伪差（C1/C2·D1/D2 同址别名 P93 + `mov 0x941f7xx` vs `mov 0x0` 重定位
    // 占位），p167 真差集 0。
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

        // ORIG 0x083348e6..0x083348fb：`movsbw step,%ax; cmp curStep,%ax; jle`
        // ⇒ 局部与比较均为 short 有符号（jbe 是无符号版，失配）。
        short curStep = static_cast<short>(user->GetCurConditionEventStep());
        if (curStep < static_cast<short>(step))
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
                // ORIG 用 %esi 跨调用保活、无栈槽往返 ⇒ 原码是内联表达式（P0）。
                user->getCurCharacInvenW()->SetEventCoin(
                    user->getCurCharacInvenR()->GetEventCoin() + it->second);
                user->m_historyLog.EventCoinAdd(
                    user->getCurCharacInvenR()->GetEventCoin(), it->second,
                    static_cast<eCoinAddReason>(2));
                user->SaveMoneyCoin();
                user->SendCoin();
                continue;
            }

            CItem* item = G_CDataManager()->find_item(it->first);
            if (item != 0)
            {
                Inven_Item inv;
                // ORIG 0x08334acd：create_item 之前把 itemIdx 写进 inv+0x02（m_addInfo），
                // 其后 guard/IsSpecialItem/GetRestrictCode/ProcessSpecialItem 全部复读该
                // 成员（不再重取 it->first ⇒ p167 ptEv 多余 4 次的来源）。
                inv.m_addInfo = it->first;
                G_CDataManager()->m_itemList->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(0), inv, it->second);

                // ORIG guard（0x08334b07..+0x2df）：itemIdx==0 或 get_add_info()==0 → 不处理。
                // ORIG 为内联短路（mov$1/mov$0 岛 + test %al，无槽）；具名 bool 会多 store+cmpb。
                if (inv.m_addInfo != 0 && inv.get_add_info() != 0)
                {
                    int outType = 0;
                    int outValue = 0;
                    bool isSpecial = false;
                    if (GlobalData::s_pCeraShop->IsSpecialItem(inv.m_addInfo, outType, outValue, isSpecial))
                    {
                        // ORIG 0x08334b7f 区：特殊物品 → GetItemType() 检查。
                        // ORIG 为否定守卫（`je <else>` 直落 ProcessSpecialItem），且 itemType
                        // 不落局部（两次独立 call GetItemType）⇒ 内联调用 + 翻转块序。
                        if (!(item->GetItemType() == 0x10 || inv.m_addInfo == 1))
                        {
                            // ORIG 0x08334bb5 区：ProcessSpecialItem 发放特殊物品。
                            int outRestrict = 0;
                            int restrictCode =
                                G_CDataManager()->m_itemList->GetRestrictCode(inv.m_addInfo);
                            long curSec = GlobalData::s_systemTime_.getCurSec();
                            std::vector<Inven_Item> items;
                            WongWork::CCeraShop::stCeraShopItemParam_t param;
                            // ORIG 0x08334c66..+0x6d 为内联短路（call;test %al,%al;je），
                            // 具名 bool 会多 store+cmpb 两条（P0）。
                            if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                                    user, inv.m_addInfo, restrictCode, outRestrict, curSec, true,
                                    items, param, 0, 0, false, 1))
                            {
                                // ORIG 0x08334c73..+0x89 为 guard 形（outRestrict!=0 直落
                                // 0x11 出口），非 if/else 反向布局。
                                if (outRestrict != 0)
                                {
                                    return 0x11;
                                }
                                if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
                                {
                                    user->SetRestrictedGoods(restrictCode);
                                    GlobalData::s_pCeraShop->SaveFeaturedIdx(
                                        user, restrictCode, curSec, 0);
                                    user->SendRestrictedGoods(true, restrictCode);
                                }
                            }
                        }
                        else
                        {
                            // type==0x10 或 itemIdx==1：不做 ProcessSpecialItem；
                            // 仅 type==0x10 直接入库（0x08334d8e 区）。
                            if (item->GetItemType() == 0x10)
                            {
                                // ORIG 0x08335069..+0x71 两段式：先 `movl $-1,slot`
                                // 再独立赋值（P197③），合并初始化式会少一条 store。
                                int slot = -1;
                                slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                                    inv, static_cast<eItemAddReason>(0x25), true, true);
                                // ORIG 0x08335071..+0x82c 用 `shr $0x1f;test %al`，4.4 对
                                // `slot<0` / `(unsigned)slot>>31` 恒折 `cmpl $0;jns`（跨版本
                                // lowering，2 条差不可造，记 N）。
                                if (slot < 0)
                                {
                                    // ORIG 0x08335082 区（与 0x08334e72 同构，sender 槽不同）：
                                    // 入库失败 → msg_80/msg_81 系统邮件 + inv.reset()。
                                    char sender[30];
                                    strncpy(sender,
                                            g_scriptStringManager_.findString(
                                                4, "game_server_msg_80", 0), 0x1d);
                                    char title[256];
                                    strncpy(title,
                                            g_scriptStringManager_.findString(
                                                4, "game_server_msg_81", 0), 0xff);
                                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                        sender, inv, 0,
                                        static_cast<unsigned int>(user->getCurCharacNo()),
                                        title, static_cast<int>(strlen(title)), 0,
                                        user->GetServerGroup(), false, false);
                                    inv.reset();
                                }
                                else
                                {
                                    // ORIG 0x08335184 区：此分支无 IsCreatureItemType 判定，
                                    // 恒按 itemspace 0 发送（Ghidra C local_24<0 的 else 臂）。
                                    user->SendUpdateItem(CUser::eSendTarget_1,
                                                         static_cast<ENUM_ITEMSPACE>(0), slot);
                                }
                            }
                        }
                    }
                    else
                    {
                        // ORIG 0x08334f9e 区：非特殊物品 → 直接入库。
                        // ORIG 0x08334f99..+0x61 两段式（同 0x08335069，P197③）。
                        int slot = -1;
                        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                            inv, static_cast<eItemAddReason>(0x25), true, true);
                        // ORIG 0x08334e61..+0x61c：同 0x08335071 的 shr 形（记 N）。
                        if (slot < 0)
                        {
                            // ORIG 0x08334e72 区：入库失败 → msg_80/msg_81 系统邮件 + reset。
                            char sender[30];
                            strncpy(sender,
                                    g_scriptStringManager_.findString(
                                        4, "game_server_msg_80", 0), 0x1d);
                            char title[256];
                            strncpy(title,
                                    g_scriptStringManager_.findString(
                                        4, "game_server_msg_81", 0), 0xff);
                            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                sender, inv, 0,
                                static_cast<unsigned int>(user->getCurCharacNo()),
                                title, static_cast<int>(strlen(title)), 0,
                                user->GetServerGroup(), false, false);
                            inv.reset();
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
#include "CConditionEventManager/GetCurEventIndex.inc"
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return 0;
    return G_CDataManager()->m_conditionEvent.m_field0;
}

// ORIG T 0x083354f0 _ZN22CConditionEventManager16GetMaxRewardStepEs
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 时返回
// m_conditionEvent.m_map8.size()（+0x4da0 map 的奖励步数上限）；否则 0。
#include "CConditionEventManager/GetMaxRewardStep.inc"
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
#include "CConditionEventManager/_getRequiredParameter.inc"
{
    // b28d-52：ORIG 0x083356a1 key 槽 -0x10 系 find 的匿名实参临时（P236 内联
    // 实参），非具名局部；去掉具名 key 后 it(-0x14)/end(-0xc) 槽序对齐。
    std::pair<int, int> result;
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_conditionEvent.m_map8.find((int)step);
    if (it != G_CDataManager()->m_conditionEvent.m_map8.end())
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
#include "CConditionEventManager/_processProperDungeonClearEvent.inc"
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
#include "CConditionEventManager/CheckErrorCardForStep.inc"
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
#include "CConditionEventManager/GetConditionRewardInfo.inc"
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
    // [b29-Lane21] ORIG 0x0833547b `mov -0x20(%ebp),%eax; mov %eax,-0x28(%ebp)` =
    //   lower_bound 结果（it@-0x20）的**具名副本**（cur@-0x28，for-init 处声明）⇒
    //   独立槽 + 后续槽位整体深 4；直接以 it 循环少一槽（残差 9 行）。
    // b28d-52：ORIG 0x083354de 增量段为后置 ppEi（P28 增量侧：三槽阶梯）+
    // 具名结构拷贝（0x0833548e 逐字段 +4/+8/+0xc 后 push_back(&info)）。
    for (std::multimap<int, stConditionEventRewardInfo>::iterator cur = it;
         cur != endIt; cur++)
    {
        stConditionEventRewardInfo info = cur->second;
        out.push_back(info);
    }
}

// ORIG T 0x083358bc _ZN22CConditionEventManager16IsDoubleGoldCardEP5CUserc
// CheckErrorCardForStep 不过 → false；普通集合（useQuickPartyAdd=0）按 step 收集
// 奖励信息，IsDoubleGoldCardProcess 为真 → 双倍；否则须为快速组队
// （GetParty 非空且 is_quick_party），再以快速集合（useQuickPartyAdd=1）更新判定。
#include "CConditionEventManager/IsDoubleGoldCard.inc"
{
    // [b28c-L15] ORIG 只用一个 bool 局部（CheckErrorCardForStep 与
    //   IsDoubleGoldCardProcess 结果共用 -0xd 槽）⇒ 合并原 check/doubleGoldCard。
    bool doubleGoldCard = CheckErrorCardForStep(user, step);
    if (!doubleGoldCard)
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
#include "CConditionEventManager/IsDoubleGoldCardProcess.inc"
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
#include "CConditionEventManager/IsFreeGoldCardProcess.inc"
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
#include "CConditionEventManager/IsFreeGoldCard.inc"
{
    // b28d-52：ORIG 0x0833579e check 与 freeGoldCard 复用同一槽 -0xb
    // （P0 槽-局部对应律 ⇒ 原码单变量复用），合并为 result。
    bool result = CheckErrorCardForStep(user, step);
    if (!result)
        return false;
    short curIdx = (short)GetCurEventIndex();
    std::vector<stConditionEventRewardInfo> infos;
    GetConditionRewardInfo((char)0, infos, curIdx, step);
    result = IsFreeGoldCardProcess(infos);
    return result;
}

// ORIG T 0x08335566 _ZN22CConditionEventManager18ProcessCheckStepUpEP5CUsers
// 事件 0x71 进行中且 (short)step == m_conditionEvent.m_field0 == 2 时推进晋级流程。
#include "CConditionEventManager/ProcessCheckStepUp.inc"
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)step != field0)
        return;
    // b28d-52：ORIG 0x083355be movswl -0x1c(%ebp),%eax; cmp $0x2,%eax（寄存器
    // 比较链）；P286 单 case switch 复现该形（if 形 4.4 恒折 cmpw mem）。
    switch ((int)step)
    {
    case 2:
        _processProperDungeonClearEvent(user, step);
        break;
    default:
        break;
    }
}
