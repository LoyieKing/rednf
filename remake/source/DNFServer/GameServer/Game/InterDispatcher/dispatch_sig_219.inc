int Inter_VillageAttackedScore::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() != '\0')   // 门1：整合频道拒绝
        return 0;

    // CEnvironment+0x1a8 环境槽未在权威头建模（沿 CUserOnlyOrig.cpp:9027 先例，
    // §4.3 例外保留地址形式；ORIG 两次独立 G_CEnvironment 调用，P48）
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    if (user->getCurCharacR() == 0)                            // 门3：当前角色必须存在
        return 0;
    // ORIG：包基址槽在门 3（getCurCharacR）之后才物化。
    Inter_VillageAttackedScorePktView* pPkt = reinterpret_cast<Inter_VillageAttackedScorePktView*>(data);

    if (user->get_charac_no(-1) != pPkt->m_characNo)
        return 0;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf8);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_16);
    packet.put_int(pPkt->m_1a);
    packet.put_int(pPkt->m_1e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}