// df_game_r 还原 —— EventClassify::CEventActionMng 发奖/发信实现。
// 逐函数依据 docs/class_func_reports/EventClassify__CEventActionMng/*.md
// （process_action_send_mail 0x0810cf32 / process_coin_send 0x0810d4ae /
//   process_avatar_send 0x0810d70c / process_gold_mail_send 0x0810d83c /
//   process_mail_send 0x0810da1c）反汇编转写；类布局见 EventClassify_Action.h。
// 邮件发件人/标题缓冲按 ORIG 以 `= {0}` 清零（gcc 4.4 -O0 展开为内联循环，
// 与 ORIG rep stos 一致），再 findString(表4) + strncpy(0x1d/0xff) 填充。

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

// ============================================================================
// process_action_send_mail（ORIG 0x0810cf32）
// 返回：3=user 为空；0x11=物品表未命中；0=成功（MakeItemPacket 失败亦回 0）。
// ============================================================================
// [b28d-L14 N82附] ORIG 0x0810cf32；p167 CLEAR（41/41）；残差=槽深/寄存器分配。
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

    if (mail.m_jobMap.size() != 0)
    {
        std::map<int, std::pair<int, int> >::const_iterator it =
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
            else if (citem->IsAvatarItem() == 0)   // vtable 槽4
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
                if (citem->IsAvatarItem() == 0)
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
    mail.m_vec38.push_back(item.m_addInfo2);
    if (item.m_field1 == 2 || item.m_field1 == 3)
        mail.m_vec38.push_back((int)item.m_fieldb);
    else
        mail.m_vec38.push_back(0);

    user->SendAradEventItemLog((unsigned long)item.m_addInfo,
                               item.m_addInfo2);
    return 0;
}

// ============================================================================
// process_coin_send（ORIG 0x0810d4ae）
// m_field1c!=0：事件币直加（GetEventCoin+add_info → SetEventCoin → 日志 →
//               SaveMoneyCoin/SendCoin）；否则置 endurance=1 走 DB 邮件。
// [N18a] ORIG @0x0810d4ae 尾段 ReqDBSendNewSystemMail 实参装载：4.1 将 mail.m_gold
//   直落 scratch edx（characNo 留 eax），4.4-O0 恒把 characNo 从 eax 中转 edx 再
//   用 eax 装 gold（多一条 `mov %eax,%edx`）。p167 调用点 19/19 CLEAR（0 miss/0
//   extra），语义完整；scratch reg 方向属 4.1↔4.4 分配差异，删/加 cast 试错不可翻。
// ============================================================================
int CEventActionMng::process_coin_send(CUser* user, Action_SendMail& mail,
                                       Inven_Item& item)
{
    // [b28c-L0 N7/N18a/P75] 语义完成；残差为虚调用寄存器分配（N7）、
    // call 结果中转（N18a）与实参装载序（P75），169=170 仅 1 条中转。
    if (mail.m_field1c != 0)
    {
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + item.m_addInfo2);
        user->m_historyLog.EventCoinAdd(
            user->getCurCharacInvenR()->GetEventCoin(), item.m_addInfo2,
            (eCoinAddReason)2);
        user->SaveMoneyCoin();
        user->SendCoin();
    }
    else
    {
        item.m_fieldb = 1;                       // +0x0b word = 1
        char title[256] = {0};
        char sender[30] = {0};
        std::strncpy(sender, g_scriptStringManager_.findString(
                         4, toTString(mail.m_field30), 0), 0x1d);
        std::strncpy(title, g_scriptStringManager_.findString(
                         4, toTString(mail.m_field34), 0), 0xff);
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
// [b28d-L14 N82附] ORIG 0x0810d0ac；残差=callee-saved 独占形（ORIG push edi/esi +
// `mov %eax,%esi`）vs 4.4 槽化(-0x18)（P52/P37 边界）。
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
    // [b28c-L0 N18a/N22/P75] 语义完成；残差为 getCurCharacNo 结果中转
    // （mov %eax,%edx）与 ReqDBSendNewSystemMail 实参寄存器对调，139=140。
    Inven_Item item;
    item.m_addInfo = -1;

    char title[256] = {0};
    char sender[30] = {0};
    std::strncpy(sender, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field30), 0), 0x1d);
    std::strncpy(title, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field34), 0), 0xff);

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
// [b21] 分支极性修复：IsAvatarItem 测试改 `if (IsAvatarItem()) {avatar 死缓冲} else
//   {DB 邮件}`，对齐 ORIG `test %al,%al; je <DB>` 块序。title[256]={0}/sender[30]={0}
//   预清、findString(4,toTString)=strncpy、ReqDBSendNewSystemMail 九参（gold/no/
//   title/strlen/mailPad/coin/server_group/b/b）逐条对齐。残余 N82：ReqDBSend 实参
//   装载信封序寄存器分配异；`add $C`/`A $C` 归一伪差。无子母，封 N。
// ============================================================================
int CEventActionMng::process_mail_send(CUser* user, Action_SendMail& mail,
                                       Inven_Item& item, CItem const* citem)
{
    // [b28c-L0 N7/N18a/P75] 语义完成；残差为虚调用寄存器（call *%edx↔*%eax）、
    // call getCurCharacNo 结果中转与实参装载序，173=174。
    if (citem == 0)
        return 0x11;

    char title[256] = {0};
    char sender[30] = {0};
    std::strncpy(sender, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field30), 0), 0x1d);
    std::strncpy(title, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field34), 0), 0xff);

    // ORIG 分支极性：`test %al,%al; je <DB-mail>` —— avatar（IsAvatarItem 非 0）为
    // if-body，DB-mail 为 else；反写匹配 ORIG 块序。
    if (citem->IsAvatarItem())
    {
        // avatar 分支：ORIG 仅清零 0x18 字节局部缓冲并置 0 一个 int
        // 后返回 0（无 DB 请求）。mailPad/dummy 均为忠实还原的死存储。
        char mailPad[24] = {0};
        int dummy = 0;
    }
    else
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item,
            (unsigned int)mail.m_gold,
            (unsigned int)user->getCurCharacNo(),
            title, (int)std::strlen(title),
            (unsigned int)(signed char)mail.m_coin,
            user->GetServerGroup(), false, false);
    }
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

    switch (citem->GetItemType())   // ORIG vtable slot +0x0c
    {
    case 6:
    {
        Inven_Item item;
        // [b28d-L14 实证] ORIG 0x0810dc5a 直取 m_itemList 成员（无 getItemList
        // call，P167 差集）；make_pair 实例为 <unsigned long&,int&>（m_addInfo
        // 以 unsigned long 左值视图入参，P61/P211）。
        G_CDataManager()->m_itemList->get_lotto_item(
            citem->get_index(), reinterpret_cast<char*>(&item));
        if (item.m_addInfo == 0 || item.m_addInfo == -1)
            return 0x11;
        out.push_back(std::make_pair(
            reinterpret_cast<unsigned long&>(item.m_addInfo),
            item.m_addInfo2));
        break;
    }
    case 16:
    {
        std::vector<stStackableBoosterItemInfo_t> boosters;
        static_cast<CStackableItem*>(citem)->getBoosterItem(boosters);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosters.begin();
             it != boosters.end(); ++it)
        {
            if (it->m_itemIdx == 0)
                return 0x11;
            // ORIG make_pair<unsigned int const&, unsigned short const&>
            // （m_itemIdx 字段以 unsigned int 左值视图入参，P61/P211）
            out.push_back(std::make_pair(
                reinterpret_cast<unsigned int const&>(it->m_itemIdx),
                it->m_addInfo));
        }
        break;
    }
    default:
        return 0x17;
    }
    return 0;
}

}  // namespace EventClassify
