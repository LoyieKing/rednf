#include "Inter_OnTimeEventAckReward.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "DNFFunctionLib.h"

#include <cstring>

// data 包缓冲的组件视图（P1：组件视图 → 单指令 disp 寻址，匹配 ORIG
// movzwl 0xa(%eax) / mov 0x10(%eax) 等；裸指针加偏移生成 add 两步）
struct AckRewardView
{
    char m_pad[0xa];        // +0x00
    short m_resultNo;       // +0x0a（signed：ORIG movzwl+movswl）
    char m_pad2[0x4];       // +0x0e
    unsigned int m_accId;   // +0x10
    unsigned int m_charNo;  // +0x14
    unsigned int m_itemNo;  // +0x18
};

// ---------------------------------------------------------------------------
// Inter_OnTimeEventAckReward::dispatch_sig(CUser*, char*, int)
//   ORIG 0x084e11ec（T，size 0x38b）。vtable[0] 虚槽（ISigDispatcher 纯虚覆盖）。
//   data 指向客户端 ACK 包缓冲（+0x0a short result_no；+0x10/+0x14/+0x18 三个 uint）。
//   全程返回 0；日志走 cMyTrace 临时链式（P37：args 提前求值存 ebx/push 帧，
//   栈槽从低地址按词法序分配——与 ORIG -0x6c..-0x1c 逐槽对应）。
// ---------------------------------------------------------------------------
// [b28c-32] verdict=DIFF：N82 变参预装（ORIG cMyTrace ctor 前 %edi/%esi 保活 vararg，4.4 直压）。
int Inter_OnTimeEventAckReward::dispatch_sig(CUser* user, char* data, int len)
{
    int local_20;

    local_20 = (int)data;
    cMyTrace(__PRETTY_FUNCTION__, 0x502d, 0)(
        "[ON_TIME_EVENT] Request Reward Ack(result_no:%u)",
        ((AckRewardView*)local_20)->m_resultNo);
    if (user == 0) {
        return 0;
    }
    if (((AckRewardView*)local_20)->m_resultNo != 0) {
        if (user->get_state() > 2) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120,
                                     (unsigned char)((AckRewardView*)local_20)->m_resultNo);
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x503a, 0)(
            "[ON_TIME_EVENT] Request Reward Ack:err_wrong(m_id:%s,char_no:%u,result_no:%u)",
            NumberToString(user->get_acc_id(), 0), ((AckRewardView*)local_20)->m_accId,
            ((AckRewardView*)local_20)->m_resultNo);
        return 0;
    }
    if ((user == 0 || user->getCurCharacR() == 0 || user->get_state() < 3) &&
        (int)((AckRewardView*)local_20)->m_accId != 0) {
        SendOnTimeRewardByMail(((AckRewardView*)local_20)->m_accId,
                               ((AckRewardView*)local_20)->m_charNo,
                               ((AckRewardView*)local_20)->m_itemNo);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x11);
        cMyTrace(__PRETTY_FUNCTION__, 0x5049, 0)(
            "[ON_TIME_EVENT] Request Reward Ack:err_logout(m_id:%s,char_no:%u,result_no:%u)",
            NumberToString(user->get_acc_id(), 0), ((AckRewardView*)local_20)->m_accId,
            ((AckRewardView*)local_20)->m_resultNo);
        return 0;
    }
    if ((int)((AckRewardView*)local_20)->m_accId != user->get_charac_no(-1)) {
        return 0;
    }
    if (!user->InsertOnTimeEventReward(((AckRewardView*)local_20)->m_charNo,
                                       ((AckRewardView*)local_20)->m_itemNo)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x11);
        cMyTrace(__PRETTY_FUNCTION__, 0x505a, 0)(
            "[ON_TIME_EVENT] Request Reward Ack:err_item(m_id:%s,char_no:%u,result_no:%u)",
            NumberToString(user->get_acc_id(), 0), ((AckRewardView*)local_20)->m_accId,
            ((AckRewardView*)local_20)->m_resultNo);
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x505d, 0)(
        "[ON_TIME_EVENT] Request Reward Ack:success(m_id:%s,char_no:%u,result_no:%u)",
        NumberToString(user->get_acc_id(), 0), ((AckRewardView*)local_20)->m_accId,
        ((AckRewardView*)local_20)->m_resultNo);
    return 0;
}

// ---------------------------------------------------------------------------
// Inter_OnTimeEventAckReward::SendOnTimeRewardByMail(uint, uint, uint)
//   ORIG 0x084e1578（T，size 0x17c）。非虚。nCharacNo=0xc 槽（邮件接收角色号）、
//   itemNo=0x10 槽（find_item 参数）、count=0x14 槽（create_item 数量）。
//   Inven_Item 构造在 find_item 判空之后（ORIG +0x02b）；item.m_addInfo(+0x02)
//   即 Ghidra 的 local_60@-0x5c（create_item 后仍读该字段）。
// ---------------------------------------------------------------------------
void Inter_OnTimeEventAckReward::SendOnTimeRewardByMail(unsigned int nCharacNo, unsigned int itemNo,
                                                        unsigned int count)
{
    char body[256];
    char title[21];
    int local_10;

    local_10 = (int)G_CDataManager()->find_item((int)itemNo);
    if (local_10 == 0) {
        return;
    }
    Inven_Item item;
    item.m_addInfo = (int)itemNo;
    G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, (int)count);
    if (item.m_addInfo != 0 || item.get_add_info() != 0) {
        strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_132", 0), 0x14);
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_134", 0), 0xff);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0, nCharacNo, body, (int)strlen(body), 0,
            (ENUM_SERVER_GROUP)0, false, false);
        item.reset();
    }
    return;
}
