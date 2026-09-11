// [b28d-lane2] verdict=DIFF ndiff=78（ORIG 0x084dc97a）：槽序/帧差(N82/P212/P221)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=46（ORIG 0x84dc97a）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_FindFactoryHub::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_FindFactoryHubPktView* pPkt = reinterpret_cast<Inter_FindFactoryHubPktView*>(data);

    if (user != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2c0, 1,
            (unsigned int)user->SetETC(0x2c0, pPkt->m_0), 0);
        if (pPkt->m_4 != 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x2bd, 1,
                (unsigned int)user->SetETC(0x2bd, pPkt->m_0), 0);
        }

        Packet_Monitor_Find_Factory_Hub_User packet;
        packet.m_charNo = user->getCurCharacNo();      // +0x0a
        packet.m_nameLen =
            (unsigned char)strlen(pPkt->m_5);           // +0x0e 截断到字节
        strncpy(packet.m_name, pPkt->m_5, 0x1e);        // +0x0f
        packet.m_field2e = 0x2c1;                      // +0x2d（ORIG 恒写 0x2c1）
        packet.m_accId = user->get_acc_id();           // +0x2f
        packet.packetSize = 0x33;                      // ORIG 发送长度恒 0x33

        // [b28d-lane2] ORIG 084dc9f4 段：长度 0x33 先落具名 int 槽（-0xc，
        // `movl $0x33,-0xc(%ebp)` + `mov -0xc(%ebp),%edx`），GetServerProxy 结果直落
        // (%esp)（无具名 proxy 槽）。
        // P198 孪生判据：初始化式会被 4.4 折成常量（丢槽）⇒ 声明与赋值分两句。
        int sendLen;
        sendLen = 0x33;
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), sendLen);
    }
    return 0;
}