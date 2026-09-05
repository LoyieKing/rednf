
#include "Dispatchers.h"

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 定义由 CompoundEmblemDeps.cpp 提供（见 CompoundEmblemDeps.h）。
extern int LineFunc(int, const char*, int, unsigned int);

// ---------------------------------------------------------------------------
// Dispatcher_UseVendingMachine::dispatch_sig —— 自动售货机（扫地机/道具售货机）
// 购买流程还原。
//
// 依据 docs/class_func_reports/Dispatcher_UseVendingMachine/（ORIG
// dispatch_sig @ 0x0821c2e6，约 1009 指令；连同 6 个辅助 _get/_put/_notify 方法）。
//
// 说明（`IMPL` = 本文件重构决策，非 ORIG 事实）：
//   1. ORIG 的这些辅助方法是类成员（_ZNK28Dispatcher_UseVendingMachine…），
//      Dispatchers.h 类声明未含这些成员且被约定只读，故本文件以 TU 内
//      file-local 辅助函数实现，显式传 CMTRand* 与数据源上下文。
//   2. ORIG 使用 this->m_rand（CMTRand* @ +0x04，由 ctor 以 time 种子创建）。
//      Dispatchers.cpp 的 ctor 为空桩，不改；故 dispatch_sig 内新建本地 CMTRand
//      并按 ORIG ctor 语义以 time 播种，传给各辅助函数。
//   3. 外部依赖已由 producer 在权威头/TU 落地：CVendingMachineStatistic /
//      TimerVendingMachineItemNotify（VendingMachine.h）、CGlobalEffectManager
//      （CGlobalEffectManager.h）、CStackableItem::getGlobalEffectData、
//      user_creature::CCreatureItemConverter::SetEggItem（CInventory.h）。
//      本文件直调这些真实成员，不再定义本地替身。
//   4. 返回码层面的错误路径（ORIG 为 __cxa_throw ENUM_ERROR / LineFunc 断言）
//      在本文件以对应 return 值近似表达，语义与报告一致（见 game_verify_guide）。
// ---------------------------------------------------------------------------

#include <ctime>

#include <cstring>

#include <algorithm>

#include "CDataManager.h"
#include "CInventory.h"
#include "CUserCharacInfo.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "Inven_Item.h"
#include "CUser.h"
#include "GlobalData.h"
// 权威 WongWork::CGenUniqueNo 见 CInventory.h（含 genIPGNo(ENUM_IPGNO_TYPE,uint,char*) @0x084b7750），
// 故不再 include CGenUniqueNo.h（其类签名不同且与 CInventory.h 冲突）。
#include "CCeraShop.h"
#include "PacketGuard.h"
#include "CSystemTime.h"
#include "PvPTypes.h"   // 权威 CMTRand 声明（PvPTypes.h:178；CMTRand.h 不存在，类定义于 CMTRand.cpp）
#include "VendingMachine.h"   // CVendingMachineStatistic / TimerVendingMachineItemNotify 权威声明
#include "CGlobalEffectManager.h"
#include "CUserProc.h"   // APSystem::CUserProc::ClearActionAndSendtoUser 权威声明
#include "CGuildServerProxy.h"   // CEventManager::GetRepeatEvent / CEventBase::IsEventing 权威声明

/* =========================================================================
 * ORIG 嵌套类型 stPutItemResult_t —— _putItemIntoUser/_notifyItemInfo 共用输出
 * （0x10 字节；ORIG _putItemIntoUser 0x0821b71c 开头两 store +0x00/+0x04、
 *  _notifyItemInfo 0x0821c17c 读 +0x00/+0x04 实证）。
 * 声明见 Dispatchers.h（类内 struct stPutItemResult_t; 前向）。
 * ====================================================================== */
struct Dispatcher_UseVendingMachine::stPutItemResult_t
{
    unsigned long  m_itemIdx;   // +0x00
    unsigned int   m_count;     // +0x04
    int            m_result;    // +0x08
    short          m_slot;      // +0x0c
    unsigned char  m_success;   // +0x0e
};

/* =========================================================================
 * 售货机数据直接使用 CDataManager.h 权威结构（不再本地副本）：
 *   stVendingMachine_t      +0x00 m_id；+0x04 m_infoList
 *   stVendingMachineInfo_t  +0x00 m_groupNum；+0x04 m_payType；+0x08 m_payAmount；
 *                           +0x0c m_giveCount；+0x10 m_vecGive；+0x1c m_bonusCount；
 *                           +0x20 m_vecBonus；+0x2c m_lottery
 *   stVendingMachineOutput_t +0x00 m_itemId；+0x04 m_cumWeight；+0x08 m_count
 * ====================================================================== */

// ORIG T 0x0821b60a _ZNK28Dispatcher_UseVendingMachine18_getVendingMachineEj
//   遍历 G_CDataManager()->m_vendingMachine 按 m_id 匹配，返回机器指针（无则 0）。
const stVendingMachine_t* Dispatcher_UseVendingMachine::_getVendingMachine(
    unsigned int machineId) const
{
    CDataManager* dm = G_CDataManager();
    if (dm == 0)
        return 0;
    const std::vector<stVendingMachine_t>& table = dm->m_vendingMachine;
    for (size_t i = 0; i < table.size(); ++i)
    {
        if (static_cast<unsigned int>(table[i].m_id) == machineId)
            return &table[i];
    }
    return 0;
}

// ORIG T 0x0821b698 _ZNK28Dispatcher_UseVendingMachine22_getVendingMachineInfoEPK18stVendingMachine_tj
//   在 vm->m_infoList 中按栏位 id（m_groupNum）匹配，返回栏信息（无则 0）。
const stVendingMachineInfo_t* Dispatcher_UseVendingMachine::_getVendingMachineInfo(
    const stVendingMachine_t* vm, unsigned int slotId) const
{
    const std::vector<stVendingMachineInfo_t>& infos = vm->m_infoList;
    for (size_t j = 0; j < infos.size(); ++j)
    {
        if (static_cast<unsigned int>(infos[j].m_groupNum) == slotId)
            return &infos[j];
    }
    return 0;
}

/* =========================================================================
 * 辅助：按权重随机选一个给/赏物品，写入 out（Inven_Item）。
 * ==================================================================== */

// ORIG T 0x0821bf82 _ZNK28Dispatcher_UseVendingMachine12_getGiveItemEPK22stVendingMachineInfo_t
//   依 m_giveCount 权重（范围 = m_giveCount-1）经 this->m_rand 随机选 m_vecGive 中
//   一个输出；命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
//   按值返回 Inven_Item（sret，0x3d 字节；ORIG 调用处逐字段拷贝实证）。
Inven_Item Dispatcher_UseVendingMachine::_getGiveItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;           // 初始 idx = -1（“未选择”标记）
    int range = info->m_giveCount - 1;
    if (range < 0)
        return out;
    unsigned long r = (unsigned long)range;
    int pick = m_rand->randInt(r);
    const std::vector<stVendingMachineOutput_t>& vec = info->m_vecGive;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (static_cast<unsigned int>(pick) < vec[i].m_cumWeight)
        {
            out.m_addInfo = vec[i].m_itemId;
            out.m_addInfo2 = (int)vec[i].m_count;
            return out;
        }
    }
    return out;
}

// ORIG T 0x0821c058 _ZNK28Dispatcher_UseVendingMachine13_getBonusItemEPK22stVendingMachineInfo_t
//   依 m_bonusCount 权重随机选 m_vecBonus 中一个输出（事件开启检查见上版推断注释）。
//   命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
Inven_Item Dispatcher_UseVendingMachine::_getBonusItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;
    // ORIG 0x0821c058：GetRepeatEvent(0x23) → IsEventing(0) 判活，
    // 事件未开启 → 返回空 Inven_Item（m_addInfo=-1）。
    CEventManager* evMgr = GlobalData::s_event_manager;
    CEventBase* ev = evMgr ? evMgr->GetRepeatEvent(0x23) : 0;
    if (ev == 0 || !ev->IsEventing(0) || info->m_vecBonus.empty())
        return out;
    int range = info->m_bonusCount - 1;
    if (range < 0)
        return out;
    unsigned long r = (unsigned long)range;
    int pick = m_rand->randInt(r);
    const std::vector<stVendingMachineOutput_t>& vec = info->m_vecBonus;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (static_cast<unsigned int>(pick) < vec[i].m_cumWeight)
        {
            out.m_addInfo = vec[i].m_itemId;
            out.m_addInfo2 = (int)vec[i].m_count;
            return out;
        }
    }
    return out;
}

/* =========================================================================
 * 辅助：把物品/货币/胜点交给用户，填充 result。
 * ==================================================================== */
// ORIG T 0x0821b71c _ZNK28Dispatcher_UseVendingMachine16_putItemIntoUserEP5CUsermjRNS_17stPutItemResult_tE
//   把 itemIdx（0=金币/1=活动币/2=胜点/else=实物）交给用户，填充 result。
void Dispatcher_UseVendingMachine::_putItemIntoUser(
    CUser* user, unsigned long itemIdx, unsigned int count,
    stPutItemResult_t& result) const
{
    // 常量（ORIG 直接 cmpl 立即数比较，避免局部 const 数组）
    enum { PAY_MONEY = 0, PAY_EVENTCOIN = 1, PAY_WINPOINT = 2 };
    // （其它值 = 给实物物品，按 itemIdx）

    result.m_itemIdx = itemIdx;
    result.m_count   = count;
    result.m_result  = 0;
    result.m_slot    = -1;
    result.m_success = 1;

    if (result.m_itemIdx == 0xffffffffUL)
        return;

    short slot = -1;

    CUserCharacInfo* chi = static_cast<CUserCharacInfo*>(user);

    if (itemIdx == PAY_MONEY)
    {
        CInventory* invW = chi->getCurCharacInvenW();
        invW->gain_money((int)count, (eMoneyAddReason)0x15, 1, 0);
        slot = 0;
    }
    else if (itemIdx == PAY_EVENTCOIN)
    {
        CInventory* invW = chi->getCurCharacInvenW();
        int cur = invW->GetEventCoin();
        invW->SetEventCoin((unsigned int)(cur + (int)count));
        CInventory* invR = const_cast<CInventory*>(chi->getCurCharacInvenR());
        unsigned int newCoin = invR->GetEventCoin();
        // ORIG: cUserHistoryLog::EventCoinAdd((cUserHistoryLog*)(user+0x79700),coin,amount,6)
        user->m_historyLog.EventCoinAdd((int)newCoin, (int)count, (eCoinAddReason)6);
        slot = 1;
    }
    else if (itemIdx == PAY_WINPOINT)
    {
        // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c（CUser 域已补声明+实现）。
        user->gainWinPoint((int)count, (eWPAddReason)3);
        slot = 2;
    }
    else
    {
        // 实物物品
        CDataManager* dm = G_CDataManager();
        CStackableItem* item = static_cast<CStackableItem*>(dm->find_item((int)itemIdx));
        if (item == 0)
        {
            result.m_success = 0;
            return;
        }

        Inven_Item outItem;
        CDataManager* dm2 = G_CDataManager();
        dm2->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)0, outItem, (int)count);

        int itemType = item->GetItemType();
        if (itemType == 0x17)
        {
            // 需要全局效果：按 m_vec280（vector<stGlobalEffect_t>）逐项注册。
            // ORIG：registNewEffect(m_field0, m_field4, characName, 5, m_field8)。
            const std::vector<stGlobalEffect_t>& eff = item->getGlobalEffectData();
            const char* characName = chi->getCurCharacName();
            for (size_t e = 0; e < eff.size(); ++e)
            {
                // ORIG 0x0821b953：把 stGlobalEffect_t +0x04 按 float
                // 读取，而非将该 int 数值转换为 float。该槽由
                // createGlobalEffectInfo 保留脚本中的 IEEE-754 位模式。
                float value;
                std::memcpy(&value, &eff[e].m_field4, sizeof(value));
                GlobalData::s_globalEffectManager_->registNewEffect(
                    (unsigned int)eff[e].m_field0, value, characName, 5,
                    (long)eff[e].m_field8);
            }
            slot = -1;
        }
        else if (item->IsAvatarItem())
        {
            unsigned int characNo = chi->getCurCharacNo();
            char ipgBuf[0x18];
            WongWork::CGenUniqueNo::genIPGNo((WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)4,
                                             characNo, ipgBuf);
            char* ipg = ipgBuf;
            int usablePeriod = item->getUsablePeriod();
            CInventory* invW = chi->getCurCharacInvenW();
            slot = (short)invW->AddAvatarItem((int)itemIdx, usablePeriod, /*b*/0, /*c*/0,
                                              /*d*/(char)-1, ipg, (eAvatarItemAddReason)0xe,
                                              /*e*/0, /*f*/(unsigned short)0);
            result.m_result = 1;
        }
        else if (item->IsCreatureItem() && item->GetItemType() == 0x16)
        {
            // 生物（蛋/宠物），先 SetEggItem 再 insert
            if (outItem.m_field1 == 5 /* ITEM_TYPE_CREATURE_EQUIP */)
            {
                bool ok = user_creature::CCreatureItemConverter::SetEggItem(&outItem);
                if (ok)
                {
                    CInventory* invW = chi->getCurCharacInvenW();
                    slot = (short)invW->insertItemIntoInventory(outItem,
                                                                (eItemAddReason)0x23, 1, 0);
                    if (slot >= 0)
                    {
                        int expire = item->getExpirationDate();
                        int usable = item->getUsablePeriod();
                        CInventory* invW2 = chi->getCurCharacInvenW();
                        invW2->GetCreatureMgrW()->InsertCreatureItem(&outItem, (int)slot,
                                                                     0x23, usable, expire);
                    }
                    // ORIG 0x0821bc0d：仅 SetEggItem 成功后才标记结果类型 7；
                    // 转换失败或非 creature equipment 直接沿用默认结果类型 0。
                    result.m_result = 7;
                }
            }
        }
        else
        {
            // 普通实物
            // ORIG 0x0821bc1c 起：Cera 特殊物品处理 + 受限商品（防 hack）完整路径。
            //   IsSpecialItem → 非 0x10（普通 stackable）时 ProcessSpecialItem 分发，
            //   restrictCode 非 0 且未受限 → SetRestrictedGoods + SaveFeaturedIdx +
            //   SendRestrictedGoods；ProcessSpecialItem 返回 true 且 rc!=0 表示特殊
            //   处理已发物，直接结束（result 保持默认）。
            bool specialHandled = false;   // ORIG -0x21(%ebp)
            int specialType = 0, specialValue = 0;
            bool specialFlag = false;
            if (GlobalData::s_pCeraShop->IsSpecialItem((int)itemIdx, specialType,
                                                       specialValue, specialFlag)
                && item->GetItemType() != 0x10)
            {
                std::vector<Inven_Item> items;    // ORIG -0x70(%ebp)
                int rc = 0;                       // ORIG -0x74(%ebp)
                int restrictCode =
                    G_CDataManager()->getItemList()->GetRestrictCode((int)itemIdx);
                long curSec = GlobalData::s_systemTime_.getCurSec();
                WongWork::CCeraShop::stCeraShopItemParam_t param;  // ORIG -0x8c(%ebp)
                bool ok = GlobalData::s_pCeraShop->ProcessSpecialItem(
                    user, (int)itemIdx, restrictCode, rc, curSec, true,
                    items, param, 0, 0, false, 1);
                if (ok && rc != 0)
                    return;   // ORIG：特殊处理已放物品，直接结束
                if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
                {
                    user->SetRestrictedGoods(restrictCode);
                    GlobalData::s_pCeraShop->SaveFeaturedIdx(user, restrictCode,
                                                             (int)curSec, 0);
                    user->SendRestrictedGoods(true, restrictCode);
                }
                specialHandled = true;   // ORIG：跳过普通 insert
            }
            if (!specialHandled)
            {
                CInventory* invW = chi->getCurCharacInvenW();
                slot = (short)invW->insertItemIntoInventory(outItem,
                                                            (eItemAddReason)0x23, 1, 0);
                if (slot == -1)
                    result.m_success = 0;
            }
        }
    }

    result.m_slot = slot;
    if (result.m_success != 0)
    {
        CVendingMachineStatistic* st = GetInstanceVendingMachineStatistic();
        if (st)
            st->addOutputItem(result.m_itemIdx, result.m_count);
    }
}

/* =========================================================================
 * 辅助：给用户发"物品已放入"计时通知（若该物品尚未出现在用户拥有的列表）。
 * ==================================================================== */
// ORIG T 0x0821c17c _ZNK28Dispatcher_UseVendingMachine15_notifyItemInfoEPK22stVendingMachineInfo_tP5CUserRKNS_17stPutItemResult_tE
//   在 info->m_lottery（vector<pair<int,int>>）中查找 (itemIdx,count)；命中才
//   registTimer 延迟发放（ORIG _notifyItemInfo 汇编：pair 构造 + find + operator==
//   判等后跳 registTimer）。
void Dispatcher_UseVendingMachine::_notifyItemInfo(
    const stVendingMachineInfo_t* info, CUser* user,
    const stPutItemResult_t& result) const
{
    std::pair<int, int> key((int)result.m_itemIdx, (int)result.m_count);
    const std::vector<std::pair<int, int> >& lot = info->m_lottery;
    if (std::find(lot.begin(), lot.end(), key) != lot.end())
    {
        unsigned int uid = user->GetUID();
        TimerVendingMachineItemNotify::registTimer((int)uid, result.m_itemIdx,
                                                   result.m_count, 5);
    }
}

/* =========================================================================
 * dispatch_sig —— 主入口。
 * ==================================================================== */
int Dispatcher_UseVendingMachine::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
        return 0;
    if (user->get_state() != 3)
        return 0;
    if (user->CheckInTrade())
        return 0;

    unsigned int machineId = 0;
    unsigned int slotId    = 0;
    short        pos       = -1;

    if (!packet.get_int(machineId))
        return LineFunc(0xd029, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(slotId))
        return LineFunc(0xd02a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(pos))
        return LineFunc(0xd02b, __PRETTY_FUNCTION__, 0, 0);

    const stVendingMachine_t* vm = _getVendingMachine(machineId);
    if (vm == 0)
        return LineFunc(0xd02f, __PRETTY_FUNCTION__, 0, 0);
    const stVendingMachineInfo_t* info = _getVendingMachineInfo(vm, slotId);
    if (info == 0)
        return LineFunc(0xd033, __PRETTY_FUNCTION__, 0, 0);

    // 随机源：ORIG ctor 以 time 种子构造 this->m_rand（Dispatchers.cpp ctor 已 new）。
    // 给/赏物品数组（两槽：槽 0 给物品、槽 1 赏物品）。
    Inven_Item give[2];
    give[0].reset();
    give[1].reset();
    give[0].m_addInfo = -1;
    give[1].m_addInfo = -1;
    for (int slot = 0; slot < 2; ++slot)
    {
        if (slot == 0)
        {
            // ORIG 只在“全局效果已存在”的特殊物品上重抽；普通物品、
            // 无效物品指针以及全局效果为空时均保留原选择。
            unsigned int retry = 0;
            do
            {
                if (retry > 2)
                    break;  // ORIG 此处抛 ENUM_ERROR(1)
                Inven_Item picked = _getGiveItem(info);
                give[slot] = picked;
                if (give[slot].m_addInfo == -1)
                    break;
                CItem* git = G_CDataManager()->find_item(give[slot].m_addInfo);
                if (git != 0 && git->GetItemType() == 0x17 &&
                    GlobalData::s_globalEffectManager_ != 0 &&
                    GlobalData::s_globalEffectManager_->getEffectNum() != 0)
                {
                    give[slot].m_addInfo = -1;
                    give[slot].m_addInfo2 = 0;
                    ++retry;
                }
            } while (give[slot].m_addInfo == -1);
        }
        else
        {
            // 赏物品
            Inven_Item picked = _getBonusItem(info);
            give[slot] = picked;
        }
    }

    // 空位检查：多背包空间与货架空位（ORIG 逐背包区段 check_empty_count 1..10）。
    bool invFull = true;
    CUserCharacInfo* chi = static_cast<CUserCharacInfo*>(user);
    {
        const int zones[7] = { 1, 2, 3, 10, 9, 5, 6 };
        for (int z = 0; z < 7; ++z)
        {
            CInventory* inv = chi->getCurCharacInvenW();
            if (inv->check_empty_count((Inven_Item::ITEM_TYPE)zones[z], 2) != 1)
            {
                invFull = false;
                break;
            }
        }
        if (invFull)
        {
            CInventory* inv = chi->getCurCharacInvenW();
            if (inv->GetRemainCapacity((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) > 1)
                invFull = false;
        }
    }
    if (invFull)
        return 0;   // ORIG throw ENUM_ERROR(4)：背包满

    // 扣款
    int remainingMoney = 0;
    if (info->m_payType == 0)
    {
        CInventory* invW = chi->getCurCharacInvenW();
        if (invW->use_money(info->m_payAmount, (eMoneySubReason)0x1c, 1) != 1)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        remainingMoney = invW->get_money();
    }
    else if (info->m_payType == 1)
    {
        const CInventory* invR = chi->getCurCharacInvenR();
        unsigned int total = invR->getTotalCoin();
        if (total < (unsigned int)info->m_payAmount)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        for (int i = 0; i < info->m_payAmount; ++i)
        {
            CInventory* invW = chi->getCurCharacInvenW();
            invW->UseCoin((eCoinSubReason)6);
        }
        const CInventory* invR2 = chi->getCurCharacInvenR();
        remainingMoney = (int)invR2->getTotalCoin();
    }
    else if (info->m_payType == 2)
    {
        unsigned int wp = chi->GetWinPoint();
        if (wp < (unsigned int)info->m_payAmount)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        user->UseWinPoint(info->m_payAmount, (eWPSubReason)4);   // ORIG CUser::UseWinPoint（含 WPSub 日志）
        remainingMoney = chi->GetWinPoint();
    }
    else
    {
        // 以指定栏物品（pos 指向的）支付：payType 即所需支付物品 idx。
        const CInventory* invR = chi->getCurCharacInvenR();
        Inven_Item payItem = invR->GetInvenSlot((INVEN_TYPE)1, pos);
        if (payItem.m_addInfo != info->m_payType)
            return 0;   // ORIG throw ENUM_ERROR(0x11)
        if (payItem.m_fieldd < info->m_payAmount)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        CInventory* invW = chi->getCurCharacInvenW();
        if (invW->delete_item((INVEN_TYPE)1, (int)pos, info->m_payAmount,
                              (eItemDelReason)0x1c, 1) != 1)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        const CInventory* invR2 = chi->getCurCharacInvenR();
        remainingMoney = invR2->GetInvenSlot((INVEN_TYPE)1, pos).m_fieldd;
    }

    CVendingMachineStatistic* vst = GetInstanceVendingMachineStatistic();
    if (vst)
        vst->incUseCount();

    // 输出
    stPutItemResult_t result[2];
    for (int i = 0; i < 2; ++i)
        _putItemIntoUser(user, give[i].m_addInfo, give[i].m_addInfo2, result[i]);

    // ORIG：首个给物品失败时，以给物品表的第一项再尝试一次；随后仅
    // 对首项按 (itemIdx != -1 && count != 0) 触发延迟通知。
    if (result[0].m_success != 1 && !info->m_vecGive.empty())
    {
        const stVendingMachineOutput_t& fallback = info->m_vecGive.front();
        _putItemIntoUser(user, fallback.m_itemId, fallback.m_count, result[0]);
    }
    if (result[0].m_itemIdx != 0xffffffffUL && result[0].m_count != 0)
        _notifyItemInfo(info, user, result[0]);

    // 回包：header 0xda
    PacketGuard resp;
    resp.put_header(1, 0xda);
    resp.put_byte(1);
    resp.put_short(pos);
    resp.put_int(remainingMoney);
    for (int i = 0; i < 2; ++i)
    {
        resp.put_item_idx(result[i].m_itemIdx);
        resp.put_int((int)result[i].m_count);
    }
    int idx = resp.get_index();
    int cnt = 0;
    resp.put_short(0);
    for (int i = 0; i < 2; ++i)
    {
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_slot != -1 &&
            result[i].m_success != 0)
        {
            CInventory* inv = const_cast<CInventory*>(chi->getCurCharacInvenR());
            inv->MakeItemPacket((INVEN_TYPE)result[i].m_slot, (int)result[i].m_slot, resp);
            ++cnt;
        }
    }
    resp.put_short(idx, cnt);
    resp.finalize(true);
    user->Send(resp);
    APSystem::CUserProc::ClearActionAndSendtoUser(
        user, (APSystem::_ActionGroupIndex)0x27, 0, (ENUM_PACKETCLASS)0);
    return 0;
}

// ============================================================================
// ORIG 0x08233dbc W _ZN28Dispatcher_UseVendingMachine7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0821c2e6，本文件上方)
//   及 6 个 _get/_put/_notify 辅助方法。忠实转写空返回，勿臆造。
int Dispatcher_UseVendingMachine::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                          ParamBase& /*param*/)
{
    return 0;
}

// ORIG 0x08233db2 W _ZN28Dispatcher_UseVendingMachine4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x08233db2..0x08233dbc）；ORIG 的包读取在
//   dispatch_sig 内完成（get_int/get_int/get_short 直读 PacketBuf）。
int Dispatcher_UseVendingMachine::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;
}