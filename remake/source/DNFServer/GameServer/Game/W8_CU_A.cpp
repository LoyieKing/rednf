// ============================================================================
// W8_CU_A.cpp —— CUser Wave8 CUserOnlyOrig 残余批 A（抽奖/时序判定族，7 符号）
//
// 来源：docs/class_func_reports/CUser/{IsOldLotteryItem,lottery_item,
//       multibox_lottery_item}.md（## 汇编 + ## 反编译 C 全文）
//       + objdump 直转：
//         CUser::IsOldLotteryItem      T 0x08673342 (0x60)
//         CUser::inc_multiboxLotteryItemFailCnt W 0x080fdf56 (0x23)
//         CUser::SellItemTimeChecker   W 0x08230256 (0x55)
//         CUser::GetItemTimeChecker    W 0x082302ac (0x1f0)
//         CUser::GetItemSerialChecker  W 0x0823066a (0x14f)
//         CUser::lottery_item          T 0x086733a2 (0x15c6)
//         CUser::multibox_lottery_item T 0x08674968 (0x115d)
//
// NOTE(NEEDS_FIX, owner=CDataManager 域批次)：CDataManager::find_lottery_use_cost
// 当前声明/实现返回 bool（isNeedMoney 转发），ORIG mangled
// _ZNK12CDataManager21find_lottery_use_costEj 返回 int（花费数值——返回类型不编码
// 于 mangled）。本 TU 按正确语义以 int 使用其结果；宿主把签名改回 int 后无需再动
// 本文件（bool→int 整型提升编译可通过，值路径待修复）。
//
// NOTE(NEEDS_HEADER, owner=宿主裁决)：CParty.h 以 char 占位承载 +0xb24

// CBattle_Field(0xcd0) 子对象区（见 CParty.h 注释），ORIG ctor 0x082a5516 已实证。
// GetItemSerialChecker 的 party 分支需要读取该子对象的 m_visitedGotoFlag/
// m_hellMapIdx/get_cur_map()。本 TU 采用仓库既有成熟范式（DisPatcherCore.cpp:3333、
// ExpertJobOnlyMgr.cpp:86 同型 reinterpret_cast 视图）；待宿主把 CParty.h 战场区
// 正式嵌入 CBattle_Field 后统一 cutover 为具名子对象访问。
//
// 本 TU 外部依赖符号（仅声明无实现；owner 注记）：
//   void CInGameAdvertisementManager::CheckFailMultiBoxLottery(CUser*,
//          std::vector<std::pair<Inven_Item,bool>>&)  ORIG T 0x080fcb3e，
//     owner = W8_LongTail.LT08_AdvertisementMisc（新 TU 承载全族实现）。
//   CInGameAdvertisementManager* InGameADManagerInstance()
//     ORIG T _Z23InGameADManagerInstancev @0x080fc9e9，owner 同上。
//   （二者的类最小视图在本 TU 末尾声明区给出，仅供指针上成员调用，
//     不产生任何静态数据/定义，无 ODR 风险。）
// ============================================================================

#include <cstring>

#include "CUser.h"
#include "CMCAPManager.h"
#include "CBoosterGage.h"
#include "CAvatarItemMgr.h"
#include "CCeraShop.h"
#include "CGenUniqueNo.h"
#include "Inven_Item.h"
#include "CItemList.h"
#include "CInventory.h"
#include "CItemGenerator.h"       // random_option::CRandomOptionItemHandle
#include "GamePODTypes.h"         // stAvatarExpansionInfo_t
#include "CSystemTime.h"
#include "GlobalData.h"           // s_pCeraShop / g_randomOptionHandle / s_systemTime_
#include "CBattle_Field.h"
#include "CParty.h"
// ORIG 全局空槽物品（B 0x943ddc0；唯一 owner 定义于 CAccountCargo.cpp）
extern Inven_Item g_emptySlot;
#include "GameWorld.h"

// ---------------------------------------------------------------------------
// 最小外部声明区（owner=W8_LongTail.LT08_AdvertisementMisc；本 TU 无定义）
// ---------------------------------------------------------------------------
class CInGameAdvertisementManager
{
public:
    void CheckFailMultiBoxLottery(
        CUser* user,
        std::vector<std::pair<Inven_Item, bool> >& rewardList);
};
extern CInGameAdvertisementManager* InGameADManagerInstance();

// ---------------------------------------------------------------------------
// CInGameAdvertisementManager::CheckFailMultiBoxLottery —— ORIG 0x080fcb3e
// _ZN27CInGameAdvertisementManager24CheckFailMultiBoxLotteryEP5CUserRSt6vectorISt4pairI10Inven_ItembESaIS5_EE
// 报告 docs/class_func_reports/CInGameAdvertisementManager/CheckFailMultiBoxLottery.md：
//   若 rewardList 全部为 multibox 广告箱物品(idx ∈ {0x1e3c,0x1e3d,0x1e3e})，
//   inc_multiboxLotteryItemFailCnt；连续失败 >2 时发送 put_header(0,0x1c6) 空通知包；
//   任一非广告物品 → set_multiboxLotteryItemFailCnt(0)。单例 owner=GameMain.cpp。
// ---------------------------------------------------------------------------
void CInGameAdvertisementManager::CheckFailMultiBoxLottery(
    CUser* user,
    std::vector<std::pair<Inven_Item, bool> >& rewardList)
{
    if (user == 0 || rewardList.empty()) {
        return;
    }

    bool allAdItems = false;
    for (std::size_t i = 0; i < rewardList.size(); ++i) {
        const unsigned int idx =
            static_cast<unsigned int>(rewardList[i].first.m_addInfo);
        if (idx != 0x1e3cu && idx != 0x1e3du && idx != 0x1e3eu) {
            allAdItems = false;                              // ORIG +0xd1 置零后跳出
            break;
        }
        allAdItems = true;
    }

    if (!allAdItems) {
        user->set_multiboxLotteryItemFailCnt(0);             // 0x080fdf44
        return;
    }
    if (user->inc_multiboxLotteryItemFailCnt() <= 2) {       // 0x080fdf56
        return;
    }

    PacketGuard out;
    out.put_header(0, 0x1c6);
    out.finalize(true);
    user->Send(out);
}

// ===========================================================================
// 1) CUser::IsOldLotteryItem(unsigned int) @0x08673342 —— 报告全文直转
// ===========================================================================
bool CUser::IsOldLotteryItem(unsigned int itemIdx)
{
    if (itemIdx < 0x1d57u) {
        if (itemIdx < 0x1d4cu) {
            if (itemIdx < 0x1bc6u) {
                return false;
            }
            if ((itemIdx > 0x1bf5u) && ((itemIdx - 0x1c15u) > 4u)) {
                return false;
            }
        }
    } else if (itemIdx < 0x1e5du) {
        if ((itemIdx < 0x1e44u) && ((itemIdx - 0x1dcau) > 5u)) {
            return false;
        }
    } else if ((itemIdx - 0x1ebau) > 1u) {
        return false;
    }
    return true;
}


// ===========================================================================
// 3) CUser::SellItemTimeChecker(unsigned int nowSec, unsigned int interval)
//    @0x08230256。字段：m_mcap.m_sellTimeChkBaseSec(+0x3c)/
//    m_mcap.m_sellTimeHitCount(+0x40)（本批于 CMCAPManager.h 拆 pad 具名化，
//    已广播默许）。
// ===========================================================================
unsigned int CUser::SellItemTimeChecker(unsigned int nowSec, unsigned int interval)
{
    if ((nowSec - m_mcap.m_sellTimeChkBaseSec) > interval) {
        // 超窗重置：基准秒=本次时间、计数清零
        m_mcap.m_sellTimeChkBaseSec = nowSec;
        m_mcap.m_sellTimeHitCount = 0;
    } else {
        ++m_mcap.m_sellTimeHitCount;
    }
    return static_cast<unsigned int>(m_mcap.m_sellTimeHitCount);
}

// ===========================================================================
// 3b) CUser::inc_multiboxLotteryItemFailCnt() @0x080fdf56
// ===========================================================================
int CUser::inc_multiboxLotteryItemFailCnt()
{
    m_multiboxLotteryFailCnt += 1;
    return m_multiboxLotteryFailCnt;
}

// ===========================================================================
// 4) CUser::GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
//                              unsigned short val2, unsigned int rangeSec)
//    @0x082302ac。双通道时序差窗（hack 时序检测）：
//    超 rangeSec → 重置；窗口内更新 min/max 并写 |max-min| 绝对值到 diff 字段。
// ===========================================================================
int CUser::GetItemTimeChecker(unsigned int nowSec, unsigned short val1,
                              unsigned short val2, unsigned int rangeSec)
{
    short v1 = static_cast<short>(val1);
    short v2 = static_cast<short>(val2);

    if ((nowSec - m_mcap.m_timeChkBaseSec) > rangeSec) {
        // 重置分支（ORIG 写序：diff 清零 → 计数清零 → 基准秒与四端点）
        m_mcap.m_timeChkDiffA = 0;
        m_mcap.m_timeChkDiffB = 0;
        m_mcap.m_timeChkCount = 0;
        m_mcap.m_timeChkBaseSec = nowSec;
        m_mcap.m_timeChkMinA = v1;
        m_mcap.m_timeChkMaxA = v1;
        m_mcap.m_timeChkMinB = v2;
        m_mcap.m_timeChkMaxB = v2;
    } else {
        ++m_mcap.m_timeChkCount;
        if ((static_cast<int>(m_mcap.m_timeChkMinA) - v1) > 0) {
            m_mcap.m_timeChkMinA = v1;               // 取更小端
        }
        if ((static_cast<int>(m_mcap.m_timeChkMaxB) - v2) > 0) {
            m_mcap.m_timeChkMaxB = v2;               // 取更大端
        }
        if ((static_cast<int>(m_mcap.m_timeChkMaxA) - v1) < 0) {
            m_mcap.m_timeChkMaxA = v1;
        }
        if ((static_cast<int>(m_mcap.m_timeChkMinB) - v2) < 0) {
            m_mcap.m_timeChkMinB = v2;
        }

        int a = static_cast<int>(
            static_cast<short>(m_mcap.m_timeChkMaxA - m_mcap.m_timeChkMinA));
        m_mcap.m_timeChkDiffA =
            static_cast<short>((a ^ (a >> 31)) - (a >> 31));  // abs

        int b = static_cast<int>(
            static_cast<short>(m_mcap.m_timeChkMaxB - m_mcap.m_timeChkMinB));
        m_mcap.m_timeChkDiffB =
            static_cast<short>((b ^ (b >> 31)) - (b >> 31));
    }
    return m_mcap.m_timeChkCount;
}

// ===========================================================================
// 5) CUser::GetItemSerialChecker(unsigned short serialIdx,
//                                unsigned short serialVal, int monsterIdx)
//    @0x0823066a。序列号差窗连续命中计数：idx 差∈[-23,23] 且 val 差∈[-17,17]
//    才保持计数；越窗重置。命中条件 = party 存在且 party->isMonsterDrop_item
//    且战场未做 visited-goto 校验（bf.m_visitedGotoFlag==0 或
//    bf.m_hellMapIdx != bf.get_cur_map()）。
// ===========================================================================
int CUser::GetItemSerialChecker(unsigned short serialIdx, unsigned short serialVal,
                                int monsterIdx)
{
    int idxDiff = static_cast<short>(serialIdx);
    int valDiff = static_cast<short>(serialVal);

    bool inWindow =
        ((idxDiff - static_cast<int>(m_mcap.m_serialLastIdx)) >= -23 &&
         (idxDiff - static_cast<int>(m_mcap.m_serialLastIdx)) <= 23 &&
         (valDiff - static_cast<int>(m_mcap.m_serialLastVal)) >= -17 &&
         (valDiff - static_cast<int>(m_mcap.m_serialLastVal)) <= 17);

    if (!inWindow) {
        // 重置：命中计数清零、记录本次基准对
        m_mcap.m_serialHitCount = 0;
        m_mcap.m_serialLastIdx = serialIdx;
        m_mcap.m_serialLastVal = serialVal;
        return m_mcap.m_serialHitCount;
    }

    bool hit = false;
    CParty* p = static_cast<CParty*>(GetParty());
    if (p != NULL && p->isMonsterDrop_item(monsterIdx)) {
        // 战场子对象视图（CParty+0xb24，占位期经 m_padBattleField1 访问）
        CBattle_Field& bf =
            *reinterpret_cast<CBattle_Field*>(p->m_padBattleField1);
        hit = (bf.m_visitedGotoFlag == 0) || (bf.m_hellMapIdx != bf.get_cur_map());
    }
    if (hit) {
        ++m_mcap.m_serialHitCount;
    }
    return m_mcap.m_serialHitCount;
}

// ===========================================================================
// 6) CUser::lottery_item(int slot) @0x086733a2 —— 使用槽上 lottery 物品开奖。
//    金币奖励 / avatar / 生物蛋 / 强化三档概率（ServerParameterScript 内嵌表）/ 
//    useCost 扣款通知 / 主响应包 / 全服强化播报。全程照报告转写。
// ===========================================================================
int CUser::lottery_item(int slot)
{
    const CInventory& invenR = *getCurCharacInvenR();
    CInventory& invenW = *getCurCharacInvenW();
    CDataManager* dm = G_CDataManager();
    ServerParameterScript& sp = dm->m_serverParameter;

    Inven_Item curItem = invenR.GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    unsigned int ticketId = static_cast<unsigned int>(curItem.m_addInfo);

    CItem* costItem = dm->find_item(ticketId);                  // local_5c
    if (costItem == NULL) {
        return 0x15;
    }
    if (get_charac_level() < costItem->getUsableLevel()) {
        return 0xe;
    }

    // NOTE(NEEDS_FIX)：find_lottery_use_cost 见文件头注记
    int useCost = dm->find_lottery_use_cost(ticketId);           // local_50
    if (useCost != 0 && invenR.get_money() < useCost) {
        return 10;
    }

    Inven_Item lotto;
    dm->getItemList()->get_lotto_item(ticketId,
                                      reinterpret_cast<char*>(&lotto));

    int resultSlotNo = 0;                                        // local_60

    // ---- 金币奖励分支：结果项无 id 且金额>0 ----
    if (lotto.m_addInfo == 0 && lotto.m_addInfo2 > 0) {
        int moneyGain = lotto.m_addInfo2;                        // local_48
        int limit =
            dm->GetMoneyLimitPerLevel(get_charac_level(),
                                      get_acc_name());           // local_44
        if (limit < (invenR.get_money() + moneyGain)) {
            const char* accIdStr = NumberToString(get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x61b9, 4);
            trace("lottery_gold_item:m_id(%s), char_no(%d), money(%d)",
                  accIdStr, getCurCharacNo(), moneyGain);
            return 0x16;
        }
        int gained =                                             // local_3c
            invenW.gain_money(moneyGain, static_cast<eMoneyAddReason>(0x17),
                              true, 0);
        if (gained < moneyGain) {
            SendMoneyFullReason(static_cast<ENUM_MONEY_FULL_REASON>(0),
                                static_cast<unsigned long>(moneyGain),
                                static_cast<unsigned long>(gained));
        }
        bool okDel =
            invenW.delete_item(INVEN_TYPE_EQUIP, slot, 1,
                               eItemDelReason_3, true);
        if (!okDel) {
            return 0x11;
        }
        PacketGuard out;                                         // local_bc
        out.put_header(1, 0x1d);
        out.put_byte(1);
        out.put_short(slot);
        out.put_short(resultSlotNo);
        out.put_int(0);
        out.put_int(gained);
        out.put_short(0);
        out.put_byte(0);
        out.put_byte(0);
        out.put_short(0);
        out.put_packet(g_emptySlot);
        out.finalize(true);
        Send(out);
        send_broadcast_lottery_gold_message(
            static_cast<unsigned long>(lotto.m_addInfo), moneyGain);
        return 0;
    }

    // ---- 结果道具抽取强化概率（costTicket 可堆叠且类型 0xd 时进入；
    //      SPS 字段与 dm 绝对偏移映射见注释：dm+0x590→sp 相对 0x528=m_508[8]…）----
    unsigned int resultId = static_cast<unsigned int>(lotto.m_addInfo);
    CItem* costTicket = dm->find_item(ticketId);                 // local_58
    CItem* resItem = dm->find_item(resultId);                    // local_54

    // ORIG 条件精写（报告 LAB_08673abd..0x70f）：
    if (costTicket != NULL && resItem != NULL) {
        bool condStack = costTicket->is_stackable() &&
                         costTicket->GetItemType() == 0xd;
        if (condStack) {
            bool upgradeGate =
                (lotto.m_field1 == '\x01') &&
                (resItem->GetItemType() != 0xb);
            if (upgradeGate) {
                int up = 0;                                      // local_38
                int roll = get_rand_int(10000);
                if (roll < sp.m_508[10]) {
                    up = sp.m_508[8] + get_rand_int(sp.m_508[11]) / 1000;
                } else if (roll < (sp.m_508[10] + sp.m_508[14])) {
                    up = sp.m_508[12] + get_rand_int(sp.m_508[15]) / 1000;
                } else {
                    up = sp.m_508[16] + get_rand_int(sp.m_508[19]) / 1000;
                }
                if (up < 0 || up > 0x1f) {
                    up = 0;
                }
                lotto.SetUpgrade(static_cast<unsigned char>(up));
            }
        }
    }

    if (resultId == 0u || resultId == 0xffffffffu) {
        return 0x11;
    }

    // ---- 插入容量预检 ----
    bool capacityOK = (invenR.QuickEmptyCount() > 0);
    if (!capacityOK) {
        if (!invenR.check_empty_count(Inven_Item::ITEM_TYPE_EQUIP, 1)) {
            return 4;
        }
        if (lotto.get_add_info() >= 2) {
            if (!invenR.check_empty_count(Inven_Item::ITEM_TYPE_2, 1)) {
                return 4;
            }
        }
        if (!invenR.check_empty_count(Inven_Item::ITEM_TYPE_3, 1)) {
            return 4;
        }
        if (!invenR.check_empty_count(Inven_Item::ITEM_TYPE_10, 1)) {
            return 4;
        }
    }

    // ---- 消耗票（id != 0x28becb 即刻删除；否则延后） ----
    if (ticketId != 0x28becbu) {
        bool delOk =
            invenW.delete_item(INVEN_TYPE_EQUIP, slot, 1,
                               eItemDelReason_3, true);
        if (!delOk) {
            return 0x11;
        }
    }

    int remainOrAdd = 0;                                         // local_30
    char ipgName[24];
    memset(ipgName, 0, sizeof(ipgName));                         // local_e0

    if (resItem != NULL && resItem->IsAvatarItem()) {
        // ---- avatar 分支 ----
        WongWork::CGenUniqueNo::genIPGNo(WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                                         static_cast<unsigned>(getCurCharacNo()),
                                         ipgName);
        int period = resItem->getUsablePeriod();
        resultSlotNo =                                            // local_60
            invenW.AddAvatarItem(static_cast<int>(resultId), period, false,
                                 '\0', '\0', ipgName,
                                 static_cast<eAvatarItemAddReason>(7), false, 0);
        if (resultSlotNo == -1) {
            return 4;
        }
        int nowSec = GlobalData::s_systemTime_.getCurSec();
        Inven_Item placed = invenR.GetInvenSlot(INVEN_TYPE_AVATAR, resultSlotNo);
        const WongWork::CAvatarItemMgr* avm = invenR.GetAvatarItemMgrR();
        remainOrAdd = avm->GetRemainDate(placed.m_addInfo2, nowSec);
        lotto.SetUpgrade(0);
    } else if (resItem != NULL && resItem->IsCreatureItem() &&
               resItem->GetItemType() == 0x16) {
        // ---- 生物蛋分支 ----
        if (lotto.m_field1 != '\x05') {
            return 0x11;
        }
        if (user_creature::CCreatureItemConverter::SetEggItem(&lotto) != 1) {
            return 0x11;
        }
        resultSlotNo =
            invenW.insertItemIntoInventory(lotto, static_cast<eItemAddReason>(3),
                                           true, false);
        if (resultSlotNo < 0) {
            return 4;
        }
        int expDate = resItem->getExpirationDate();
        int period = resItem->getUsablePeriod();
        user_creature::CCreatureMgr* cm = invenW.GetCreatureMgrW();
        cm->InsertCreatureItem(&lotto, resultSlotNo, 0x19, period, expDate);
    } else {
        // ---- 普通道具（含随机选项生成）----
        if (resItem != NULL && resItem->IsRandomOption()) {
            RandomOption* optTarget = &lotto.m_random.m_random;
            if (!GlobalData::g_randomOptionHandle->give_option(
                    resultId,
                    static_cast<ENUM_RARITY>(resItem->get_rarity()),
                    resItem->getUsableLevel(), resItem->getItemGroupName(),
                    resItem->GetRandomOptionGrade(), optTarget)) {
                return 0x11;
            }
        }
        resultSlotNo =
            invenW.insertItemIntoInventory(lotto, static_cast<eItemAddReason>(3),
                                           true, true);
        if (resultSlotNo == -1) {
            return 4;
        }
        if (ticketId == 0x28becbu) {
            bool delOk =
                invenW.delete_item(INVEN_TYPE_EQUIP, slot, 1,
                                   eItemDelReason_3, true);
            if (!delOk) {
                return 0x11;
            }
        }
        remainOrAdd = lotto.get_add_info();

        if (resItem != NULL) {
            int sellPrice = 0;                                    // local_e4
            short count = static_cast<short>(lotto.m_addInfo2);
            G_Store()->GetSellItemPrice(lotto, resItem, count, false,
                                        sellPrice);
            CValueStatistic* stat = GetInstanceValueStatistic();
            stat->AddValueStatistic(static_cast<VALUE_STATISTIC_FIELD>(7),
                                    this, static_cast<unsigned int>(sellPrice));
        }
    }

    // ---- 老 lottery 物品清行动点 ----
    if (IsOldLotteryItem(ticketId)) {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, static_cast<APSystem::_ActionGroupIndex>(4), 0,
            static_cast<ENUM_PACKETCLASS>(0));
    }

    // ---- useCost 二次扣款与货币更新包 ----
    useCost = dm->find_lottery_use_cost(ticketId);
    if (useCost != 0) {
        invenW.use_money(useCost, static_cast<eMoneySubReason>(0x2d), true);
        PacketGuard notify;                                       // local_f0
        notify.clear();
        notify.put_header(0, 0xe);
        notify.put_byte(0);
        notify.put_short(1);
        invenR.MakeItemPacket(INVEN_TYPE_EQUIP, 0, notify);
        notify.finalize(true);
        Send(notify);
    }

    // ---- 主响应包 ----
    PacketGuard resp;                                             // local_c8
    resp.put_header(1, 0x1d);
    resp.put_byte(1);
    resp.put_short(slot);
    resp.put_short(resultSlotNo);
    resp.put_int(static_cast<int>(resultId));
    resp.put_int(remainOrAdd);
    resp.put_short(static_cast<unsigned short>(lotto.m_fieldb));
    resp.put_byte(lotto.GetItemAttr());
    resp.put_byte(lotto.m_amp.getAbilityType());
    resp.put_short(lotto.m_amp.getAbilityValue());
    if (lotto.isAvatarItemType()) {
        resp.put_int(0x1e);
        const WongWork::CAvatarItemMgr* avm = invenR.GetAvatarItemMgrR();
        resp.put_binary(avm->getJewelSocketData(resultSlotNo), 0x1e);
        resp.put_int(4);
        void* expansion = avm->GetExpansionInfo(resultSlotNo);
        if (expansion == NULL) {
            stAvatarExpansionInfo_t fresh;
            fresh.init();
            resp.put_binary(reinterpret_cast<const char*>(&fresh), 4);
        } else {
            resp.put_binary(static_cast<char*>(expansion), 4);
        }
    }
    resp.put_packet(lotto);
    resp.finalize(true);
    Send(resp);

    if (lotto.GetUpgrade() != 0) {
        m_historyLog.UpgradeItem(static_cast<int>(resultId), 0,
                                 lotto.GetUpgrade(), true);
    }

    // ---- 强化成功全服播报（含稀度上限豁免表）----
    bool eligibleToShout = false;
    bool havePair = (costTicket != NULL && resItem != NULL);
    if (havePair && costTicket->is_stackable() &&
        costTicket->GetItemType() == 0xd) {
        int cap = sp.m_558;                          // rarity 上限豁免（dm+0x5c0）
        int rarity = resItem->get_rarity();
        if (!(rarity < cap)) {
            eligibleToShout = false;                 // cap<=rarity 直接免播
        } else if (resItem->IsAvatarItem()) {
            if (sp.m_55c[resItem->GetItemType()] != 0) {
                eligibleToShout = false;             // 特例列表命中的 avatar 免播
            } else {
                eligibleToShout =
                    (sp.m_578 > static_cast<int>(lotto.GetUpgrade()));
            }
        } else {
            eligibleToShout =
                (sp.m_578 > static_cast<int>(lotto.GetUpgrade()));
        }
    }

    if (eligibleToShout) {
        bool skipByPlace = (getMoveSpace() ==
                            static_cast<UserSpace::T>(1)) ||
                           (resItem != NULL && resItem->IsRandomOption());
        if (!skipByPlace) {
            PacketGuard shout;                                    // local_100
            shout.put_header(0, 0x56);
            shout.put_byte(2);
            shout.put_byte(1);
            shout.put_short(
                static_cast<unsigned short>(get_unique_id() & 0xffff));
            shout.put_int(static_cast<int>(resultId));
            shout.put_byte(lotto.GetUpgrade());
            shout.finalize(true);
            G_GameWorld()->send_all(shout);
        }
    }
    return 0;
}

// ===========================================================================
// 7) CUser::multibox_lottery_item(int boxSlot, int keySlot) @0x08674968
// ===========================================================================
int CUser::multibox_lottery_item(int boxSlot, int keySlot)
{
    const CInventory& invenR = *getCurCharacInvenR();
    CInventory& invenW = *getCurCharacInvenW();
    CDataManager* dm = G_CDataManager();

    Inven_Item boxItem = invenR.GetInvenSlot(INVEN_TYPE_EQUIP, boxSlot);
    unsigned int boxItemId = static_cast<unsigned int>(boxItem.m_addInfo);

    std::vector<std::pair<Inven_Item, bool> > results;            // local_7c
    int ret;
    if (boxItemId == 0x2061u || boxItemId == 0x289843u) {
        ret = process_use_unlocked_multibox(boxSlot, keySlot, results);
    } else {
        ret = process_use_locked_multibox(boxSlot, keySlot, results);
    }
    if (ret != 0) {
        if (ret == 0x13) {
            CBoosterGage* gageFail =
                reinterpret_cast<CBoosterGage*>(GetCharacExpandData(
                    (ENUM_CHARAC_EXPAND_TYPE)11));
            gageFail->send_error_state_packet(this, boxItemId);
            return -1;
        }
        return ret;
    }

    CBoosterGage* gage =
        reinterpret_cast<CBoosterGage*>(GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)11));
    const bool isFirst = CBoosterGage::cal_booster_gage(boxItemId, this) != 0;
    if (isFirst) {
        gage->log_booster_gage_bonus_reward(this, results);
    }

    PacketGuard out;                                              // local_88
    out.put_header(1, 0xd3);
    out.put_byte(1);
    out.put_short(boxSlot);
    out.put_short(keySlot);
    out.put_short(static_cast<int>(results.size()));

    std::vector<std::pair<int, int> > updatedSlots;               // local_94

    for (std::size_t i = 0; i < results.size(); ++i) {
        Inven_Item item = results[i].first;
        int newSlot = -1;                                         // local_48
        int remainOut = 0;                                        // local_a0
        unsigned int itemId = static_cast<unsigned int>(item.m_addInfo);

        if (itemId == 1u) {
            // ---- 事件金币项 ----
            int amount = item.m_addInfo2;
            if (amount > 0 && amount < 1000) {
                int addAmount = amount;
                if (isFirst) {
                    addAmount =
                        static_cast<int>(dm->m_boosterGage.m_cMultiboxCount) *
                        amount;
                }
                unsigned int coinNow =
                    invenR.GetEventCoin() +
                    static_cast<unsigned int>(addAmount);
                invenW.SetEventCoin(coinNow);
                coinNow = invenR.GetEventCoin();
                m_historyLog.EventCoinAdd(static_cast<int>(coinNow), addAmount,
                                          static_cast<eCoinAddReason>(2));
                SaveMoneyCoin();
                SendCoin();
                out.put_short(-1);
                out.put_int(item.m_addInfo);
                out.put_int(amount);
                out.put_short(static_cast<unsigned short>(item.m_fieldb));
                out.put_byte(item.GetItemAttr());
                out.put_packet(item);
            }
            continue;
        }

        CItem* resItem = dm->find_item(itemId);                   // local_38
        if (resItem == NULL) {
            return 0x11;
        }

        if (resItem->IsAvatarItem()) {
            char ipgName[24];
            memset(ipgName, 0, sizeof(ipgName));
            WongWork::CGenUniqueNo::genIPGNo(
                WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                static_cast<unsigned>(getCurCharacNo()), ipgName);
            int period = resItem->getUsablePeriod();
            newSlot = gage->AddAvatarItem(updatedSlots, isFirst, this, item,
                                          period, false, '\0', static_cast<char>(-1),
                                          ipgName,
                                          static_cast<eAvatarItemAddReason>(7),
                                          remainOut);
            if (newSlot == -1) {
                newSlot = -1;                                     // 保持 ORIG 语义
            }
        } else if (resItem->IsCreatureItem() &&
                   resItem->GetItemType() == 0x16) {
            // ---- 生物蛋 ----
            if (item.m_field1 != '\x05') {
                newSlot = -1;
            } else if (
                user_creature::CCreatureItemConverter::SetEggItem(&item) == 1) {
                char ipgName[24];
                memset(ipgName, 0, sizeof(ipgName));
                WongWork::CGenUniqueNo::genIPGNo(
                    WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_1,
                    static_cast<unsigned>(getCurCharacNo()), ipgName);
                int expiry = resItem->getExpirationDate();
                int period = resItem->getUsablePeriod();
                newSlot = gage->InsertCreatureItem(updatedSlots, isFirst, this,
                                                   item,
                                                   static_cast<eItemAddReason>(3),
                                                   true, false, period, expiry,
                                                   ipgName);
            } else {
                newSlot = -1;
            }
        } else {
            // ---- 普通 / 特殊商城品 ----
            int specialType = 0;                                  // local_d8[0]
            int specialValue = 0;                                 // local_d8[1]
            bool specialFlag = false;
            if (GlobalData::s_pCeraShop->IsSpecialItem(itemId, specialType,
                                                     specialValue,
                                                     specialFlag)) {
                CItem* specItem = dm->find_item(itemId);
                int kind = specItem->GetItemType();
                if (kind == 0x10 || kind == 0x22) {
                    newSlot = gage->insertItemIntoInventory(
                        updatedSlots, isFirst, this, item,
                        static_cast<eItemAddReason>(3), true);
                } else {
                    int restrictCode =
                        dm->getItemList()->GetRestrictCode(itemId);
                    long curSec = GlobalData::s_systemTime_.getCurSec();
                    std::vector<Inven_Item> produceVec;            // local_ec
                    WongWork::CCeraShop::stCeraShopItemParam_t param;  // local_104
                    bool handled = GlobalData::s_pCeraShop->ProcessSpecialItem(
                        this, itemId, restrictCode, specialType, curSec, true,
                        produceVec, param, '\0', '\0', isFirst, 1);
                    if (handled) {
                        if (restrictCode != 0) {
                            newSlot = -1;
                        }
                        if (restrictCode != 0 &&
                            !IsRestrictedGoods(restrictCode)) {
                            SetRestrictedGoods(restrictCode);
                            GlobalData::s_pCeraShop->SaveFeaturedIdx(
                                this, restrictCode, curSec, 0);
                            SendRestrictedGoods(true, restrictCode);
                        }
                    }
                    newSlot = 0xffff;      // 特殊品不落背包槽（ORIG 写回 0xffff）
                }
            } else {
                newSlot = gage->insertItemIntoInventory(
                    updatedSlots, isFirst, this, item,
                    static_cast<eItemAddReason>(3), true);
            }

            if (newSlot != -1) {
                remainOut = item.get_add_info();
                int price = 0;                                    // local_108
                short count = static_cast<short>(item.m_addInfo2);
                G_Store()->GetSellItemPrice(item, resItem, count, false, price);
                if (isFirst) {
                    price *= static_cast<int>(dm->m_boosterGage.m_cMultiboxCount);
                }
                CValueStatistic* stat = GetInstanceValueStatistic();
                stat->AddValueStatistic(static_cast<VALUE_STATISTIC_FIELD>(7),
                                        this, static_cast<unsigned int>(price));
            }
        }

        if (newSlot == -1) {
            out.put_short(-1);
            out.put_int(item.m_addInfo);
            out.put_int(item.get_add_info());
        } else {
            out.put_short(newSlot);
            out.put_int(item.m_addInfo);
            out.put_int(remainOut);
        }
        out.put_short(static_cast<unsigned short>(item.m_fieldb));
        out.put_byte(item.GetItemAttr());
        out.put_packet(item);
    }

    out.put_byte(isFirst ? 1 : 0);
    out.finalize(true);
    Send(out);

    for (std::size_t i = 0; i < updatedSlots.size(); ++i) {
        SendUpdateItem(eSendTarget_1,
                       static_cast<ENUM_ITEMSPACE>(updatedSlots[i].second),
                       updatedSlots[i].first);
    }

    InGameADManagerInstance()->CheckFailMultiBoxLottery(this, results);

    for (std::size_t i = 0; i < results.size(); ++i) {
        if (results[i].second != 0) {
            PacketGuard shout;                                    // local_114
            shout.put_header(0, 0x56);
            shout.put_byte(4);
            shout.put_byte(1);
            shout.put_short(
                static_cast<unsigned short>(get_unique_id() & 0xffff));
            shout.put_int(results[i].first.m_addInfo);
            shout.put_int(static_cast<int>(boxItemId));
            shout.put_byte(results[i].first.GetUpgrade());
            shout.finalize(true);
            G_GameWorld()->send_all(shout);
        }
    }

    use_multibox_statistic(boxItemId);
    return 0;
}
