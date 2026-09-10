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
