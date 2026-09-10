int Dispatcher_EventCreateDnfRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x7b3;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG：<3 与 >4 各重读一次 get_state（P48 无 CSE）。
    if (user->get_state() < 3 || user->get_state() > 4)
    {
        return 0;
    }

    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    if (event != 0 && event->IsEventing((CUser*)0))
    {
        char reward = user->GetEventCreateDnfReward();
        if (reward == 1 || event->IsRewardCondition() == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x258, 1);
            LogManager::logFormat(
                1, "localjapan/Arad_PacketDispatcher.cpp", __PRETTY_FUNCTION__, 0x7d6,
                "[Taiwan, Event] condition error m_id:%d", user->get_acc_id());
        }
        else
        {
            user->SetEventCreateDnfReward(true);
            DBInsertCreateDnfUserInfo::makeRequest(user->GetUID(), user->get_acc_id());
            event->SendReward(user);
        }

        PacketGuard guard;
        ((InterfacePacketBuf*)&guard)->put_header(0, 0x22d);
        guard.put_short((int)event->GetRate());
        guard.put_byte(0);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }

    int eventing = (event != 0) ? (int)event->IsEventing((CUser*)0) : 0;
    LogManager::logFormat(
        1, "localjapan/Arad_PacketDispatcher.cpp", __PRETTY_FUNCTION__, 0x7c1,
        "[Taiwan, Event] stayEvent=%s eventing:%d",
        (event == 0) ? "nullptr" : "ok", (int)(eventing & 0xff));
    return 0;
