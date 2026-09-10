void CCommunityServerProxy::SendLogin(CUser* user, SIG_LOAD_PVP_BUDDY* sig)
{
    Packet_Community_Login pkt;
    pkt.m_id = user->get_acc_id();
    pkt.gameUserInfo.user_m_id = user->get_acc_id();
    pkt.gameUserInfo.server_id = user->GetServerGroup();
    pkt.gameUserInfo.channel_no = G_CEnvironment()->get_channel_no();
    pkt.gameUserInfo.m_uid = user->GetUID();
    pkt.gameUserInfo.charac_no = user->getCurCharacNo();
    pkt.gameUserInfo.m_growType = user->getCurCharacGrowType();
    pkt.gameUserInfo.m_level = user->get_charac_level();
    pkt.gameUserInfo.m_job = user->get_charac_job();
    pkt.gameUserInfo.m_sex = user->getSex();
    memcpy(pkt.gameUserInfo.m_name, user->getCurCharacName(), 0x1d);
    pkt.buddyCount = sig->m_buddyCount;
    memcpy(pkt.buddies, sig->m_buddies, 0x560);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
