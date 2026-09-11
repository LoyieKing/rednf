// ---------------------------------------------------------------------------
// Inter_OnTimeEventAckReward::dispatch_sig(CUser*, char*, int)
//   ORIG 0x084e11ec（T，size 0x38b）。vtable[0] 虚槽（ISigDispatcher 纯虚覆盖）。
//   data 指向客户端 ACK 包缓冲（+0x0a short result_no；+0x10/+0x14/+0x18 三个 uint）。
//   全程返回 0；日志走 cMyTrace 临时链式（P37：args 提前求值存 ebx/push 帧，
//   栈槽从低地址按词法序分配——与 ORIG -0x6c..-0x1c 逐槽对应）。
// ---------------------------------------------------------------------------
// [b28d-solo-Inter_OnTimeEventAckReward] verdict=AE（check_tu_routed.sh clean 口径；p167 27/27 CLEAR）。
//   修法：get_charac_no 比较点**字段侧删 (int) cast**、右侧同型化 (unsigned int)：
//   ORIG 单条 `mov 0x10(%eax),%ebx` 直落 callee-saved 复现；4.4 对「unsigned 字段 + (int) cast」
//   恒发 `mov mem,%eax; mov %eax,%ebx` 两段中转（探针 /tmp/b28d_s309_probe.cpp f1 vs f3）。
//   变参预装：全表达式临时链形（P227）已复现 ORIG ctor 前 %edi/%esi/%ebx 保活，非 N82。
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
    if (((AckRewardView*)local_20)->m_accId != (unsigned int)user->get_charac_no(-1)) {
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