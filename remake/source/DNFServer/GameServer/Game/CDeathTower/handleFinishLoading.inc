int CDeathTower::handleFinishLoading(CUser* user)
{
    // ORIG 0x8466024：正向 && 短路（cmpl$-1;je ERR / call valid;test al;je ERR
    // 无 xor），ERR=return 0 尾岛，body fall-through（范式17）。
    int slot = m_party->GetMemberSlotNo(user);
    if (slot != -1 && m_party->checkValidUser(slot))
    {
        if (m_playData.checkEnterStartMap(slot))
        {
            user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
        }
        m_playData.setMemberReady(slot, true);

        // ORIG 0x84660a3：get_member_count 结果免槽直入 arg（0x84660b1 `mov %eax,0x4(%esp)`）。
        if (m_playData.checkAllMemberReady(m_party->get_member_count()))
        {
            CPacketHandler packet;
            packet.makeFinishLoading();    // ORIG：全员 ready 后广播加载完成包
            m_party->send_to_party(packet);
        }
        return 1;
    }
    return 0;
