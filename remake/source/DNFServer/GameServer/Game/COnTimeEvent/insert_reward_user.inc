bool COnTimeEvent::insert_reward_user(CUser* user)
{
    if (user == 0)
        return false;

    char logDir[256] = { '\0' };
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    register unsigned int characNo = user->get_charac_no(-1);
    register char* accStr = NumberToString(user->get_acc_id(), 0);
    // [b28d-51] P221/P227：互斥 sibling 块同类日志对象槽深方向 —— 具名对象 4.4 反
    // （首块 -0x20/次块 -0x28），改全表达式临时链复现 ORIG（首块 -0x28/次块 -0x20）。
    CMyFileLog(__PRETTY_FUNCTION__, 0x75)(
        logDir, "On Time Event:On Request Reward m_id(%s) charac_no(%u)",
        accStr, characNo);

    if (get_ontime_id() > user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.insert(user->get_acc_id(), user->get_charac_no(-1),
                             get_ontime_id());
        user->set_ontime_last_recv_idx(get_ontime_id());
        user->set_update_ontime_last_recv_idx();
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0);
        return true;
    }
    else
    {
        char errDir[256] = { '\0' };
        sprintf(errDir, "./log/%s/OnTimeEvent",
                G_CEnvironment()->get_file_name());

        register unsigned int characNo2 = user->get_charac_no(-1);
        register char* accStr2 = NumberToString(user->get_acc_id(), 0);
        register unsigned int lastIdx = user->get_ontime_last_recv_idx();
        register unsigned int ontimeId = get_ontime_id();
        CMyFileLog(__PRETTY_FUNCTION__, 0x89)(
            errDir,
            "On Time Event:Reward(no.%u user-%u)error m_id(%s)charac_no(%u)",
            ontimeId, lastIdx, accStr2, characNo2);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x12);
        return false;
    }
