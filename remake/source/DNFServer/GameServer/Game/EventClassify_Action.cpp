// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventClassify_Action/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
// [b28d-solo-EventClassify_Action] ORIG 0x0810cf32，381=381 AE（p167 41/41 CLEAR）。
//   本件修法：①mail.m_itemIdx 先落具名 int itemIdx（ORIG -0x18）、find_item(itemIdx)；
//   ②IsAvatarItem 两处改正极性 `if (IsAvatarItem()) {avatar} else {insert}`（ORIG
//     `test %al,%al; je <else>`）；③PacketGuard 内删 sendOk 预清局部，改
//     `if (!MakeItemPacket(...)) return 0;` 内联（ORIG `call; xor $1; test; je`）。
#include "EventClassify_Action/process_action_send_mail.inc"
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
    int itemIdx = mail.m_itemIdx;      // +0x20
    int addInfo = mail.m_addInfo;      // +0x24

    if (mail.m_jobMap.size() != 0)
    {
        std::map<int, std::pair<int, int> >::const_iterator it =
            mail.m_jobMap.find(user->get_charac_job());
        if (it != mail.m_jobMap.end())
        {
            itemIdx = it->second.first;           // +4 pair.first
            addInfo = it->second.second;          // +8 pair.second
        }
    }
    if (addInfo == 0)
        addInfo = 1;

    int slot = -1;
    item.m_addInfo = itemIdx;          // +0x02（item idx）
    item.set_add_info(addInfo);        // +0x07 m_addInfo2

    if (item.m_addInfo == 1)           // 金币（cav）→ 直发流程
    {
        process_coin_send(user, mail, item);
    }
    else
    {
        CItem* citem = G_CDataManager()->find_item(itemIdx);
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
            else if (citem->IsAvatarItem())        // vtable 槽4
            {
                slot = process_avatar_send(user, item, citem);
                if (slot == -1)
                    failed = true;
            }
            else
            {
                slot = user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)0xd, true, true);
                if (slot == -1)
                    failed = true;
            }

            if (!failed && flag)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe);
                if (citem->IsAvatarItem())
                {
                    packet.put_byte(1);
                    packet.put_short(1);
                    if (!user->getCurCharacInvenR()->MakeItemPacket(
                            (INVEN_TYPE)2, slot, packet))
                        return 0;
                }
                else
                {
                    packet.put_byte(0);
                    packet.put_short(1);
                    if (!user->getCurCharacInvenR()->MakeItemPacket(
                            (INVEN_TYPE)1, slot, packet))
                        return 0;
                }
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
// [b28d-solo-EventClassify_Action] ORIG 0x0810d4ae，169=169 AE（p167 19/19 CLEAR）。
//   旧判词记的「characNo eax→edx 中转」系旧 .o 假象：fresh 编译后尾段
//   `call getCurCharacNo; mov 0x10(%ebp),%edx; mov 0x28(%edx),%edx` 与 ORIG 全等。
// ============================================================================
#include "EventClassify_Action/process_coin_send.inc"
                                       Inven_Item& item)
{
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
// [b28d-solo-EventClassify_Action] ORIG 0x0810d70c，79=79 AE。
//   本件修法：①ipg/usablePeriod 去具名局部改内联实参（P0/P200）⇒ 复现 ORIG
//     esi(ipg)/ebx(usablePeriod)/edi(item.m_addInfo) callee-saved 独占；
//   ②GetRemainDate 的 addInfo2 实参改匿名嵌套 sret
//     `GetRemainDate(GetInvenSlot(2,slot).m_addInfo2, now)`（P278）⇒ ipgBuf@-0x75、
//     sret@-0x5d 槽序与帧 0x9c 与 ORIG 全等（具名 slotItem 时两者互换、NEAR）。
#include "EventClassify_Action/process_avatar_send.inc"
                                         CItem const* citem)
{
    char ipgBuf[24];
    int slot = -1;
    slot = user->getCurCharacInvenW()->AddAvatarItem(
        item.m_addInfo, citem->getUsablePeriod(),
        /*b*/ false, /*c*/ 0, /*d*/ (char)-1,
        WongWork::CGenUniqueNo::genIPGNo(
            (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)7,
            (unsigned int)user->getCurCharacNo(), ipgBuf),
        (eAvatarItemAddReason)2, /*e*/ false, /*f*/ 0);

    if (slot != -1)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        item.m_addInfo2 =                                   // +0x07
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetRemainDate(
                user->getCurCharacInvenR()->GetInvenSlot(2, slot).m_addInfo2,
                now);
        item.SetUpgrade(0);
    }
    return slot;
}

// ============================================================================
// process_gold_mail_send（ORIG 0x0810d83c）
// 全新 Inven_Item（idx 置 -1）携带 mail.m_gold(+0x28) 走 DB 金币邮件。
// ============================================================================
#include "EventClassify_Action/process_gold_mail_send.inc"
{
    // [b28d-solo-EventClassify_Action] ORIG 0x0810d83c，139=139 AE。
    //   旧判词记的 getCurCharacNo eax→edx 中转系旧 .o 假象；fresh 编译后
    //   尾段 `mov 0x10(%ebp),%edx; mov 0x28(%edx),%edx` 与 ORIG 全等。
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
// [b28d-solo-EventClassify_Action] ORIG 0x0810da1c，173=173 AE。
//   分支极性 `if (IsAvatarItem()) {avatar 死缓冲} else {DB 邮件}` 与
//   title[256]={0}/sender[30]={0} 预清、findString(4,toTString)=strncpy、
//   ReqDBSendNewSystemMail 九参装载逐条对齐（旧 N82 判词系旧 .o 假象，已撤销）。
// ============================================================================
#include "EventClassify_Action/process_mail_send.inc"
                                       Inven_Item& item, CItem const* citem)
{
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
// [b28d-solo-EventClassify_Action] ORIG 0x0810dc5a，176 vs 170（差 6 条=唯一残差），
//   N 封顶。残差=case 16 循环内 `if (it->m_itemIdx == 0) return 0x11;` 的 0/1 物化：
//   ORIG(4.1) 发「`call ptEv; mov (%eax),%eax; test; je L1; call ptEv(丢弃);
//   mov $0; jmp L2; L1: mov $1; L2: test %al,%al; je`」= 分支+常量且条件表达式二次
//   求值；4.4 恒折 `test; sete %al; test; je`（一次求值）。已试 `==0` / `!` /
//   具名 bool / while 四形，sete 恒不变 ⇒ setcc-fold 跨版本负例（P197④ 族）。
//   可修部分已落：CStackableItem* stackable 具名局部（ORIG -0xc）复现
//   `mov -0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` 两条。
// ============================================================================
#include "EventClassify_Action/getResultItemFromLottery.inc"
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
        CStackableItem* stackable = static_cast<CStackableItem*>(citem);
        std::vector<stStackableBoosterItemInfo_t> boosters;
        stackable->getBoosterItem(boosters);
        for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it =
                 boosters.begin();
             it != boosters.end(); ++it)
        {
            // [N2-S4 2026-09-11] 复现 ORIG 的条件结果 0/1 物化（gap 6→1）：
            // ORIG 0x0810ddb1 `test %eax,%eax; je L1; <重算表达式>; mov $0x0,%eax;
            // jmp L2; L1: mov $0x1,%eax; L2: test %al,%al; je`（4.1 对带副作用的
            // bool 上下文二次求值）；4.4 直接初始化 `bool x = (p->m == 0)` 恒折
            // 单条 sete（our=171），改 if/else 显式赋值可复现「分支+常量」物化
            // （our=175）。负结论：三元 `? true : false`(171)、int if/else(175 同形)。
            // 残余 +1 = 4.4 不复现 4.1 的二次求值（operator-> 重调），跨版本封顶。
            bool emptyItem;
            if (it->m_itemIdx == 0)
                emptyItem = true;
            else
                emptyItem = false;
            if (emptyItem)
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
