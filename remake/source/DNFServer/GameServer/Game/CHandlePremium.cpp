// ============================================================================
// df_game_r —— WongWork::CHandlePremium 实现。
// 依据 docs/class_func_reports/WongWork__CHandlePremium/{handleSetUserPremium,
// handleNotifyPremiumInfo,handleSetUserPCRoom,handlePremiumEffect}.md + ORIG 反汇编。
// 权威声明见 CHandlePremium.h。
// ============================================================================

#include "CHandlePremium.h"

#include <ctime>

#include "CGameManager.h"   // CMailBoxHelper（ReqDBSendNewMail）+ ENUM_DUNGEON_MODE 守卫
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "GlobalData.h"
#include "TimerQueue.h"
#include "SPremiumInfo.h"
#include "CPremiumLetheManager.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"

namespace WongWork
{

// ---- handleSetUserPremium —— ORIG 0x086af7e8 (T) ----
// type==5 恒 AddPremium；start==0 或 end==0 则 RemovePremium；否则仅在
// s_systemTime_.getCurSec()<end 时 AddPremium。recalc 时 ReCalcAdvantage+RecalcAdditionalInfo。
void CHandlePremium::handleSetUserPremium(CUser* user, ENUM_PREMIUM_TYPE type,
                                          long start, long end, int flag, bool recalc)
{
    CUserPremium* premium = user->GetPremiumInfoW();
    int iType = (int)type;
    if (iType == 5)
    {
        premium->AddPremium(iType, (int)start, (int)end, flag);
    }
    else if (start == 0 || end == 0)
    {
        premium->RemovePremium(iType);
    }
    else
    {
        if (GlobalData::s_systemTime_.getCurSec() < end)
            premium->AddPremium(iType, (int)start, (int)end, flag);
    }
    if (recalc)
    {
        premium->ReCalcAdvantage();
        premium->RecalcAdditionalInfo(user);
    }
}

// ---- handleNotifyPremiumInfo —— ORIG 0x086af8b4 (T) ----
// 组 0x42 高级会员通知包：header(0,0x42)/short(2)/byte(type)/int(remainSec)。
void CHandlePremium::handleNotifyPremiumInfo(CUser* user, int type, int remainSec)
{
    PacketGuard packet;
    packet.put_header(0, 0x42);
    packet.put_short(2);
    packet.put_byte((unsigned char)type);
    packet.put_int(remainSec);
    packet.finalize(true);
    user->Send(packet);
}

// ---- handleSetUserPCRoom —— ORIG 0x086b02fe (T) ----
// PC 房高级会员设置入口：isAffectedPremium+handleSetUserPremium 设置 + TimerQueue
// 定时 + handleNotifyPremiumInfo/handlePremiumEffect + 租赁物品包。
void CHandlePremium::handleSetUserPCRoom(CUser* user, long start, long end)
{
    // [b23S3C] P48 去缓存：无 remain 槽——ORIG 每使用点重取 (end-curSec)（7 条/处
    // ×5）；InsertTimer 实参全内联（4.1 右→左求值序 acc_id→diff→uid→G_TQ 实证，
    // 具名局部改序即偏差）；affected bool 槽 movb 直落（ORIG `mov %al,-0x1d` 无
    // 三目物化）+ `if (!affected)` 让 effect 块走 body、timer 块成 je 岛（P191）；
    // put_byte 双分支常实参复制（ORIG 两 call 序列，非 setne 汇聚）。
    bool affected = user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc);
    handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)0xc, start, end, 0, true);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if ((int)(end - curSec) < 0x93a80 && 600 < (int)(end - curSec))
    {
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)7,
                                    (int)(end - curSec) - 600, 0xc,
                                    user->get_acc_id());
    }
    if (user->get_state() > 2)
    {
        handleNotifyPremiumInfo(user, 0xc, (int)(end - curSec));
        if (!affected)
        {
            handlePremiumEffect(user, (ENUM_PREMIUM_TYPE)0xc);
            user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
        }
        else
        {
            if ((int)(end - curSec) < 0x93a80 && 600 < (int)(end - curSec))
            {
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                            (int)user->GetUID(),
                                            (TIMER_MESSAGE)4,
                                            (int)(end - curSec), 0xc,
                                            user->get_acc_id());
            }
        }
    }
    PacketGuard packet;
    packet.put_header(0, 0x1aa);
    if (user->HasCurCharacRentalItem(1))
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.finalize(true);
    user->Send(packet);
}

// ---- handlePremiumEffect —— ORIG 0x086afc32 (T) ----
// 逐 premium 项处理：到期前定时、到期后发优势物品/金币、每日检查定时、
// 收尾 RecalcAdditionalInfo/SendNotiPacket + ProcPremiumFatigue/SendFatigue/SendCoin。
void CHandlePremium::handlePremiumEffect(CUser* user, ENUM_PREMIUM_TYPE type)
{
    CUserPremium* premium = user->GetPremiumInfoW();
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (!user->IsPremiumUser())
        return;

    std::vector<SUserPremiumInfo> infoList = premium->GetPremiumInfoList((int)type);  // ORIG 0x86afc6e 传 type 参数

    for (size_t i = 0; i < infoList.size(); ++i)
    {
        const SUserPremiumInfo& info = infoList[i];
        int iType = info.m_type;
        if (curSec < info.m_start)          // ORIG 0x086afd3f：未开始（+0x4）→ 定时
        {
            // 未开始：注册开始定时（msg 6）。
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                        (TIMER_MESSAGE)6, info.m_start - curSec,
                                        iType, user->get_acc_id());
        }
        else
        {
            if (user->getCurCharacLastPlayTick() <= info.m_end)
            {
                // 到期且上次游玩不晚于结束：发优势物品 + 金币。
                std::vector<std::pair<int, int> > items = premium->GetAdvantageItem(iType);
                for (size_t j = 0; j < items.size(); ++j)
                {
                    int itemIdx = items[j].first;
                    int count = items[j].second;
                    Inven_Item item;
                    CItem* ci = G_CDataManager()->find_item(itemIdx);
                    if (ci == 0)
                        continue;
                    ci->make_item(item);
                    item.m_addInfo = itemIdx;                  // +0x02
                    item.m_field1 = (unsigned char)(itemIdx >> 0x10);  // +0x01 高 16 位
                    item.m_addInfo2 = (item.m_field1 == 1)
                                      ? user->GetSeedFromDate() : count;  // +0x07
                    int insertRet = user->getCurCharacInvenW()->insertItemIntoInventory(
                        item, (eItemAddReason)0xe, true, true);
                    if (insertRet < 0)
                    {
                        CMailBoxHelper::ReqDBSendNewMail(
                            user, item, 0, (unsigned int)user->getCurCharacNo(), true, 0, 0);
                    }
                    else
                    {
                        user->SendUpdateItemList(CUser::eSendTarget_1,
                                                 (ENUM_ITEMSPACE)0, insertRet);
                    }
                }
                if (iType != 9999)
                {
                    CInventory* invR = const_cast<CInventory*>(user->getCurCharacInvenR());
                    int coin = (int)invR->GetCoin()
                             + g_SPremiumInfo[iType].m_premiumCount;
                    user->getCurCharacInvenW()->SetCoin((unsigned int)coin);
                    int curCoin = (int)invR->GetCoin();
                    int add = g_SPremiumInfo[iType].m_premiumCount;
                    user->m_historyLog.CoinAdd(curCoin, add, (eCoinAddReason)4);
                }
            }

            // 每日定时（仅在 premium 定义需每日检查时）。
            if (g_SPremiumInfo[iType].m_checkTerm != 0 &&
                info.m_end > curSec)
            {
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                            (TIMER_MESSAGE)4, info.m_end - curSec,
                                            iType, user->get_acc_id());
            }
            if (premium->IsNeedCheckEveryday())
            {
                time_t t = (time_t)curSec;
                struct tm tmv;
                localtime_r(&t, &tmv);
                tmv.tm_mday += 1;
                tmv.tm_hour = 0;
                tmv.tm_min = 0;
                tmv.tm_sec = 0;
                time_t nextDay = mktime(&tmv);
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, (int)user->GetUID(),
                                            (TIMER_MESSAGE)5, (int)(nextDay - t),
                                            iType, user->get_acc_id());
            }
        }
    }

    if (user->IsHavePremiumAdvantage() && (int)type != 9999)
    {
        premium->RecalcAdditionalInfo(user);
        user->SendNotiPacket(CUser::eSendTarget_1, (ENUM_NOTIPACKET)2, 1);
    }
    user->ProcPremiumFatigue();
    user->SendFatigue();
    user->SendCoin();
    if ((int)type == 9999)
        user->setCurCharacLastPlayTick((long)curSec);
}


// ---- handlePremiumTimeout —— ORIG T 0x086afa7c (handlePremiumTimeout.md) ----
// CheckPremiumTimeout 返回 0xc（PC 房类）→ setPremiumState(0xc,3)+通知；
// 其它非 0 → handleSetUserPremium(type,0,0,0,1) + RemoveRestrictedGoods +
// 角色在线时 ProcPremiumFatigue/SendFatigue + g_SPremiumInfo[type] 分支
// （kind==1 且 field64>0 → SetNeedCheckOverEquipItem+SendNotiPacket 组；
//  0x53/0x57 → 表情重算；0x21 → Lethe 标志）+ 通知。
void CHandlePremium::handlePremiumTimeout(CUser* user)
{
    SUserPremiumInfo info = user->GetPremiumInfo()->CheckPremiumTimeout();
    int timeoutType = info.m_type;
    if (timeoutType == 0xc)
    {
        user->GetPremiumInfoW()->setPremiumState((ENUM_PREMIUM_TYPE)0xc,
                                                 (ENUM_PREMIUM_STATE)3);
        handleNotifyPremiumInfo(user, 0xc, 0);
        return;
    }
    if (timeoutType == 0)
        return;

    handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)timeoutType, 0, 0, 0, true);
    // ORIG 0x086afb4c：mov -0x10(%ebp) = info.m_flag(+0xc)（非 m_type）。
    user->RemoveRestrictedGoods(info.m_flag);
    if (user->getCurCharacR() != 0)
    {
        user->ProcPremiumFatigue();
        user->SendFatigue();
        if (g_SPremiumInfo[timeoutType].m_kind == 1)
        {
            if (g_SPremiumInfo[timeoutType].m_field64 > 0)
                user->SetNeedCheckOverEquipItem();
            user->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
        }
        if (timeoutType == 0x53 || timeoutType == 0x57)
        {
            user->ReCalcChattingEmoticon();
            user->SendChattingEmoticon();
        }
        else if (timeoutType == 0x21)
        {
            user->SetOneDayLetheFlag(
                CPremiumLetheManager::NOT_USE_LETHE_STATE);
            user->SetIsEndLetheAtLogin();
        }
    }
    handleNotifyPremiumInfo(user, timeoutType, 0);
}

}  // namespace WongWork