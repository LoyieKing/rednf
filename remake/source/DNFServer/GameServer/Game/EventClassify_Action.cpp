// df_game_r 还原 —— EventClassify::CEventActionMng 发奖/发信实现。
// 逐函数依据 docs/class_func_reports/EventClassify__CEventActionMng/*.md
// （process_action_send_mail 0x0810cf32 / process_coin_send 0x0810d4ae /
//   process_avatar_send 0x0810d70c / process_gold_mail_send 0x0810d83c /
//   process_mail_send 0x0810da1c）反汇编转写；类布局见 EventClassify_Action.h。

#include "EventClassify_Action.h"

#include <cstring>

#include "CAvatarItemMgr.h"          // WongWork::CAvatarItemMgr::GetRemainDate
#include "CGenUniqueNo.h"            // WongWork::CGenUniqueNo::genIPGNo
#include "CInventory.h"
#include "CItem.h"                   // make_item / IsAvatarItem / getUsablePeriod
#include "CMailBoxHelper.h"          // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "CDataManager.h"            // G_CDataManager / find_item
#include "CUser.h"                   // Send/SendCoin/SaveMoneyCoin/m_historyLog
#include "CStackableItem.h"         // getBoosterItem / stStackableBoosterItemInfo_t
#include "GlobalData.h"              // s_systemTime_
#include "CSystemTime.h"             // getCurSec
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h" // g_scriptStringManager_ / findString
#include "SyncScriptDeps.h"          // toTString

namespace EventClassify
{

// ---- 局部工具：邮件发件人（30 字节，strncpy 0x1d）/ 标题（256 字节，
//      strncpy 0xff），均先清零后从 g_scriptStringManager_ 表 4 取串。 ----
namespace
{

void fillMailSender(char (&sender)[30], const Action_SendMail& mail)
{
    std::memset(sender, 0, sizeof(sender));
    const char* src = g_scriptStringManager_.findString(
        4, toTString(mail.m_field30), 0);
    std::strncpy(sender, src, 0x1d);
}

void fillMailTitle(char (&title)[256], const Action_SendMail& mail)
{
    std::memset(title, 0, sizeof(title));
    const char* src = g_scriptStringManager_.findString(
        4, toTString(mail.m_field34), 0);
    std::strncpy(title, src, 0xff);
}

}  // namespace

// ============================================================================
// process_action_send_mail（ORIG 0x0810cf32）
// 返回：3=user 为空；0x11=物品表未命中；0=成功（MakeItemPacket 失败亦回 0）。
// ============================================================================
int CEventActionMng::process_action_send_mail(CUser* user, Action_SendMail& mail,
                                              bool flag)
{
    if (user == 0)
        return 3;

    if (mail.m_gold > 0)
    {
        process_gold_mail_send(user, mail);
        return 0;
    }

    Inven_Item item;
    item.m_addInfo = mail.m_itemIdx;   // +0x02（item idx）
    int addInfo = mail.m_addInfo;      // +0x24

    if (!mail.m_jobMap.empty())
    {
        std::map<int, std::pair<int, int> >::iterator it =
            mail.m_jobMap.find(user->get_charac_job());
        if (it != mail.m_jobMap.end())
        {
            item.m_addInfo = it->second.first;    // +4 pair.first
            addInfo = it->second.second;          // +8 pair.second
        }
    }
    if (addInfo == 0)
        addInfo = 1;

    int slot = -1;
    item.set_add_info(addInfo);        // +0x07 m_addInfo2

    if (item.m_addInfo == 1)           // 金币（cav）→ 直发流程
    {
        process_coin_send(user, mail, item);
    }
    else
    {
        CDataManager* mgr = G_CDataManager();
        CItem* citem = mgr->find_item(item.m_addInfo);
        if (citem == 0)
            return 0x11;

        citem->make_item(item);        // vtable 槽2
        bool failed = false;

        if (mail.m_field1c != 0)       // 非 0 → coin/avatar 直发
        {
            if (user->CheckInTrade())
            {
                failed = true;
            }
            else if (!citem->IsAvatarItem())   // vtable 槽4
            {
                slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)0xd, true, true);
                if (slot == -1)
                    failed = true;
            }
            else
            {
                slot = process_avatar_send(user, item, citem);
                if (slot == -1)
                    failed = true;
            }

            if (!failed && flag)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe);
                bool sendOk = true;
                if (!citem->IsAvatarItem())
                {
                    packet.put_byte(0);
                    packet.put_short(1);
                    sendOk = user->getCurCharacInvenR()->MakeItemPacket(
                        (INVEN_TYPE)1, slot, packet);
                }
                else
                {
                    packet.put_byte(1);
                    packet.put_short(1);
                    sendOk = user->getCurCharacInvenR()->MakeItemPacket(
                        (INVEN_TYPE)2, slot, packet);
                }
                if (!sendOk)
                    return 0;
                packet.finalize(true);
                user->Send(packet);
            }
        }

        if (mail.m_field1c == 0 || failed)
            process_mail_send(user, mail, item, citem);
    }

    // 尾部发奖记录：slot / 物品代码 / add_info /（ITEM_TYPE 2、3 记剩余
    // 时间 endurance(+0x0b)，其余记 0）→ mail.m_vec38(+0x38)。
    mail.m_vec38.push_back(slot);
    mail.m_vec38.push_back(item.m_addInfo);
    mail.m_vec38.push_back(item.get_add_info());
    if (item.m_field1 == 2 || item.m_field1 == 3)
        mail.m_vec38.push_back((int)item.m_fieldb);
    else
        mail.m_vec38.push_back(0);

    user->SendAradEventItemLog((unsigned long)item.m_addInfo,
                               item.get_add_info());
    return 0;
}

// ============================================================================
// process_coin_send（ORIG 0x0810d4ae）
// m_field1c!=0：事件币直加（GetEventCoin+add_info → SetEventCoin → 日志 →
//               SaveMoneyCoin/SendCoin）；否则置 endurance=1 走 DB 邮件。
// ============================================================================
int CEventActionMng::process_coin_send(CUser* user, Action_SendMail& mail,
                                       Inven_Item& item)
{
    if (mail.m_field1c != 0)
    {
        int cur = (int)user->getCurCharacInvenR()->GetEventCoin();
        int amount = item.get_add_info();       // +0x07
        user->getCurCharacInvenW()->SetEventCoin(
            (unsigned int)(cur + amount));
        int total = (int)user->getCurCharacInvenR()->GetEventCoin();
        user->m_historyLog.EventCoinAdd(total, amount, (eCoinAddReason)2);
        user->SaveMoneyCoin();
        user->SendCoin();
    }
    else
    {
        item.m_fieldb = 1;                       // +0x0b word = 1
        char sender[30];
        char title[256];
        fillMailSender(sender, mail);
        fillMailTitle(title, mail);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item,
            (unsigned int)mail.m_gold,                       // +0x28
            (unsigned int)user->getCurCharacNo(),
            title, (int)std::strlen(title),
            (unsigned int)(signed char)mail.m_coin,          // +0x2c 低字节
            user->GetServerGroup(), false, false);
    }
    return 0;
}

// ============================================================================
// process_avatar_send（ORIG 0x0810d70c）
// genIPGNo(type7) 取 IPG 号 → AddAvatarItem(idx, usablePeriod,0,0,-1,ipg,2,0,0)
// 成功后按槽内物品与当前时间回填剩余日期到 item +0x07 并清 upgrade 位。
// ============================================================================
int CEventActionMng::process_avatar_send(CUser* user, Inven_Item& item,
                                         CItem const* citem)
{
    int slot = -1;
    char ipgBuf[24];
    char* ipg = WongWork::CGenUniqueNo::genIPGNo(
        (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
        (unsigned int)user->getCurCharacNo(), ipgBuf);

    int usablePeriod = citem->getUsablePeriod();
    slot = user->getCurCharacInvenW()->AddAvatarItem(
        item.m_addInfo, usablePeriod,
        /*b*/ false, /*c*/ 0, /*d*/ (char)-1,
        ipg, (eAvatarItemAddReason)2, /*e*/ false, /*f*/ 0);

    if (slot != -1)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        Inven_Item slotItem =
            user->getCurCharacInvenR()->GetInvenSlot(2, slot);
        int remainDate =
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                slotItem.m_addInfo2, now);
        item.m_addInfo2 = remainDate;   // +0x07
        item.SetUpgrade(0);
    }
    return slot;
}

// ============================================================================
// process_gold_mail_send（ORIG 0x0810d83c）
// 全新 Inven_Item（idx 置 -1）携带 mail.m_gold(+0x28) 走 DB 金币邮件。
// ============================================================================
int CEventActionMng::process_gold_mail_send(CUser* user, Action_SendMail& mail)
{
    Inven_Item item;
    item.m_addInfo = -1;

    char sender[30];
    char title[256];
    fillMailSender(sender, mail);
    fillMailTitle(title, mail);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item,
        (unsigned int)mail.m_gold,
        (unsigned int)user->getCurCharacNo(),
        title, (int)std::strlen(title),
        (unsigned int)(signed char)mail.m_coin,
        user->GetServerGroup(), false, false);
    return 0;
}

// ============================================================================
// process_mail_send（ORIG 0x0810da1c）
// citem 为空回 0x11；avatar 物品不入邮件（仅清局部缓冲），其余走 DB 邮件。
// ============================================================================
int CEventActionMng::process_mail_send(CUser* user, Action_SendMail& mail,
                                       Inven_Item& item, CItem const* citem)
{
    if (citem == 0)
        return 0x11;

    char sender[30];
    char title[256];
    fillMailSender(sender, mail);
    fillMailTitle(title, mail);

    if (!citem->IsAvatarItem())
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item,
            (unsigned int)mail.m_gold,
            (unsigned int)user->getCurCharacNo(),
            title, (int)std::strlen(title),
            (unsigned int)(signed char)mail.m_coin,
            user->GetServerGroup(), false, false);
    }
    // avatar 分支：ORIG 仅清零 0x18 字节局部缓冲后返回 0（无 DB 请求）。
    return 0;
}

// ============================================================================
// getResultItemFromLottery（ORIG 0x0810dc5a）
// 按 lotterycard/booster 两类物品展开为 <idx,count> 列表：
//   GetItemType()==6  → find_item + get_lotto_item → push(m_addInfo,m_addInfo2)
//   GetItemType()==16 → getBoosterItem(list) → 逐条 push(m_itemIdx,m_addInfo)
// 其它类型返回 0x17；find 失败或空结果返回 0x11。
// ============================================================================
int CEventActionMng::getResultItemFromLottery(unsigned int lotteryId,
                                              std::vector<std::pair<int, int> >& out)
{
    CItem* citem = G_CDataManager()->find_item((int)lotteryId);
    if (citem == 0)
        return 0x11;

    const int itemType = citem->GetItemType();   // ORIG vtable slot +0x0c
    if (itemType == 6)
    {
        Inven_Item item;
        G_CDataManager()->getItemList()->get_lotto_item(
            citem->get_index(), reinterpret_cast<char*>(&item));
        if (item.m_addInfo == 0 || item.m_addInfo == -1)
            return 0x11;
        out.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
    }
    else if (itemType == 16)
    {
        std::vector<stStackableBoosterItemInfo_t> boosters;
        static_cast<CStackableItem*>(citem)->getBoosterItem(boosters);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosters.begin();
             it != boosters.end(); ++it)
        {
            if (it->m_itemIdx == 0)
                return 0x11;
            out.push_back(std::make_pair(
                (int)(unsigned int)it->m_itemIdx,
                (int)(unsigned short)it->m_addInfo));
        }
    }
    else
    {
        return 0x17;
    }
    return 0;
}

}  // namespace EventClassify
