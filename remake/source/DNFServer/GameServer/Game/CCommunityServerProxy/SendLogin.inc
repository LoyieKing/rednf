// ORIG T 0x0846ca06：Packet_Community_Login 填用户信息 + PvP 好友载荷后下发。
// 字段/调用序逐条对齐报告汇编（m_id←acc_id, server_id←GetServerGroup,
// channel_no←G_CEnvironment()->get_channel_no, m_uid←GetUID,
// charac_no←getCurCharacNo, growType/level/job/sex, name memcpy 0x1d,
// buddyCount←sig[0], buddies memcpy 0x560）。
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
}