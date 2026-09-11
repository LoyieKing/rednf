// [b28d-lane2] verdict=DIFF ndiff=214（ORIG 0x084e43bc）：槽序/帧差(N82/P212/P221)+混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e43bc）：本轮删死 NumberToString 调用（p167 CLEAR）；余 P226/N18a/P30。
int Inter_Antibot_SendBufferClient::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_SendBufferClientPktView* pPkt = reinterpret_cast<Inter_Antibot_SendBufferClientPktView*>(data);
    char buffer[95000];                                // ORIG 栈缓冲 0x17338
    memset(buffer, 0, 95000);
    memcpy(buffer, data + 0xc, (int)(short)pPkt->m_8);

    CUser* target = getUser(pPkt->m_4);
    if (target == 0 || target->get_state() == 0)
    {   // 用户缺失/未登录：记录忽略日志并累计异常计数
        // ORIG 0x84e445a 单次 NumberToString（结果 %ecx 保活 → logFormat 末实参），
        // 无独立死调用（p167 EXTRA NumberToString×1 实证）。
        const char* reason;
        if (target == 0)
            reason = "NULL User";
        else
            reason = "NONE state";
        const char* kind;
        if (pPkt->m_0 == 0)
            kind = "ANTIBOT";
        else
            kind = "DPROTO";
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x5e56,
            "Ignore SendBuffertoClient [%s][size:%d]; reason:%s, QQID:%s",
            kind, (int)(short)pPkt->m_8,
            reason, NumberToString(pPkt->m_4, 0));
        if (target != 0 && (unsigned char)target->m_field4 < 5)
        {
            const unsigned char old = (unsigned char)target->m_field4;
            target->m_field4 = (char)(old + 1);
            target->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)3, 0x252,
                                              (int)old, 0);
        }
        return 0;
    }

    int packetType;                                    // 0x230=ANTIBOT / 0x231=DPROTO
    if (pPkt->m_0 == 0)
        packetType = 0x230;
    else
        packetType = 0x231;

    if (packetType == 0x230)
    {
        if (target->m_field8cf00 == 2)
        {
            target->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)2);
            target->m_field8cf00 = 3;
        }
        else if (target->m_field8cf00 != 4)
        {
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b,
                (int)(short)pPkt->m_8,
                target->m_field8cf00);
        }
    }

    PacketGuard packet;
    packet.put_header(0, packetType);
    packet.put_binary(
        buffer, (int)(short)pPkt->m_8);
    packet.finalize(false);
    char sendRet = (char)target->Send(packet);

    if (packetType == 0x230)
    {
        if (target->m_field8cf00 == 3)
        {
            target->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)3);
            target->m_field8cf00 = 4;
        }
        else if (target->m_field8cf00 != 4)
        {
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b,
                (int)(short)pPkt->m_8,
                target->m_field8cf00);
        }
    }
    else if (packetType == 0x231 &&
             (unsigned char)target->m_field4 < 5)
    {
        const unsigned char old = (unsigned char)target->m_field4;
        target->m_field4 = (char)(old + 1);
        target->m_historyLog.SendBuffertoClientLog(
            pPkt->m_0,
            (short)pPkt->m_8,
            old, sendRet != 0);
    }
    return 0;
}