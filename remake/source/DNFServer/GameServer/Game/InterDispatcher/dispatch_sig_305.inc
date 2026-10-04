// [b28d-lane2] verdict=DIFF ndiff=40（ORIG 0x084e8a42）：槽序/帧差(N82/P212/P221)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=49（ORIG 0x84e8a42）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_CheckAddFriend::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_CHECK_ADD_FRIEND* chk = reinterpret_cast<SIG_CHECK_ADD_FRIEND*>(data);
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6d7b, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6d7c;
    }

    char* recv = data;                                 // -0x10(%ebp) 缓存消息指针
    if (recv == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6d82, 5)("[%s] internal data is null.",
              __PRETTY_FUNCTION__);
        return 0x6d83;
    }

    if (chk->m_result == 0)
    {
        Packet_Monitor_Add_Buddy packet;
        packet.m_charNo = user->getCurCharacNo();      // +0x0a
        packet.m_idByChannel = user->GetUID();         // +0x0e
        memcpy(packet.m_name, chk->m_characName, strlen(chk->m_characName));
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&packet),
                          packet.packetSize);          // +0x02 长度字段
    }
    else
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x3f,
            (unsigned char)chk->m_result);
    }
    return 0;
}