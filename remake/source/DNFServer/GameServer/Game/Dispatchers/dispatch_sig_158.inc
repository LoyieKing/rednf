int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822458c（PacketDispatcher_Impl_1.cpp 0xdf01 区）
    if (G_GameWorld()->IsPVPChannel())
    {
        return 0;
    }
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0xd8);
        return 0;
    }
    COnTimeEvent* event =
        (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    if (event == 0)
    {
        return 0;
    }
    if (!event->IsEventing(0))   // 虚槽 +0x34（CEventBase::IsEventing）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x13);
        char logBuf[256] = {0};
        sprintf(logBuf, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
        unsigned int characNo = user->get_charac_no(-1);
        char* idStr = NumberToString(user->get_acc_id(), 0);
        CMyFileLog log("virtual int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser*, PacketBuf&)",
                       0xdf01);
        log(logBuf,
            "On Time Event:Request Reward IsEventing error m_id(%s) charac_no(%u)",
            idStr, characNo);
        return 0;
    }
    if (event->get_reward_state() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x13);
        // [Lane1Dispatchers 2026-09-10] N78/P214③：ORIG 该 3 处各发内联 rep stos
        //（我方 `memset` 退化为 call，rep-stos 0/3）⇒ 聚合初始化。
        char logBuf[256] = {0};
        sprintf(logBuf, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
        unsigned int characNo = user->get_charac_no(-1);
        char* idStr = NumberToString(user->get_acc_id(), 0);
        CMyFileLog log("virtual int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser*, PacketBuf&)",
                       0xdf14);
        log(logBuf,
            "On Time Event:Request Reward ENUM_ONTIME_EVENT_ON_REWARD_ON error m_id(%s) charac_no(%u)",
            idStr, characNo);
        return 0;
    }
    if (user->get_ontime_last_recv_idx() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 2);
        char logBuf[256] = {0};
        sprintf(logBuf, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
        unsigned int characNo = user->get_charac_no(-1);
        char* idStr = NumberToString(user->get_acc_id(), 0);
        CMyFileLog log("virtual int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser*, PacketBuf&)",
                       0xdf20);
        log(logBuf,
            "On Time Event:Request Reward user db error m_id(%s) charac_no(%u)",
            idStr, characNo);
        return 0;
    }
    event->insert_reward_user(user);
    return 0;
