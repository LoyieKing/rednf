int Inter_MonitorBuddyInOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorBuddyInOutPktView* pPkt = reinterpret_cast<Inter_MonitorBuddyInOutPktView*>(data);
    PacketGuard packet;
    if (pPkt->m_14 == '\x01')
        packet.put_header(0, 0x36);
    else
        packet.put_header(0, 0x37);
    packet.put_byte(
        (unsigned int)pPkt->m_12);
    packet.put_int(pPkt->m_a);
    const int local_20 = (int)strlen(pPkt->m_15);
    packet.put_int(local_20);
    packet.put_str(pPkt->m_15, local_20);
    packet.put_byte(
        (unsigned int)pPkt->m_13);
    packet.finalize(true);
    user->Send(packet);

    // [b28d-solo-InterDispatcher] P226 语句表达式：ORIG 0x084c8ff2..0x084c900f 短路物化
    // 岛（mov$1/mov$0/test al）零槽直测；具名 bool 初值+置位形 4.4 恒落槽。
    if (({ pPkt->m_33 == '\x01' &&
          user->getCurCharacR() != 0; }))
    {
        Inven_Item item;
        char* iVar2 = data + 0x15;
        const char* characName = user->getCurCharacName();
        const char* pcVar4 =
            g_scriptStringManager_.findString(4, "return_user_msg_3", 0);
        char local_16d[256];
        OS_API::snprintf(local_16d, 0xff, pcVar4, characName, iVar2);
        const ENUM_SERVER_GROUP uVar3 = user->GetServerGroup();
        const size_t sVar5 = strlen(local_16d);
        const unsigned int uVar6 = user->getCurCharacNo();
        const char* uVar7 =
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            uVar7, item, 0, uVar6, local_16d, sVar5, 0, uVar3, 0, 0);
    }
    return 0;
