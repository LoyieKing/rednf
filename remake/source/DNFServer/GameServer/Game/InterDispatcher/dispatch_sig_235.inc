// [b28d-lane2] verdict=DIFF ndiff=214（ORIG 0x084e43bc）：槽序/帧差(N82/P212/P221)+混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e43bc）：本轮删死 NumberToString 调用（p167 CLEAR）；余 P226/N18a/P30。
// [R9a08] Send 返回 bool（CUser.h:2988 串定 / ORIG AntibotSend 0x086485a7 `movzbl -0x173ad(%ebp),%eax`），
//   原 `char sendRet=(char)target->Send(...)` + `sendRet != 0` 多出 3 条 setne 物化
//   （我方 8175698 `cmpb $0x0,-0x9(%ebp); setne %al; movzbl %al,%ebx`），
//   ORIG 0x084e46ee 为 `movzbl -0x9(%ebp),%ebx` 直读 ⇒ 改 bool 局部 + 直传（值域 {0,1} 等价）。
int Inter_Antibot_SendBufferClient::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_SendBufferClientPktView* pPkt = reinterpret_cast<Inter_Antibot_SendBufferClientPktView*>(data);
    char buffer[95000];                                // ORIG 栈缓冲 0x17338
    memset(buffer, 0, 95000);
    memcpy(buffer, data + 0xc, (int)(short)pPkt->m_len);

    CUser* target = getUser(pPkt->m_uid);
    if (target == 0 || target->get_state() == 0)
    {   // 用户缺失/未登录：记录忽略日志并累计异常计数
        // ORIG 0x84e445a 单次 NumberToString（结果 %ecx 保活 → logFormat 末实参），
        // 无独立死调用（p167 EXTRA NumberToString×1 实证）。
        // [R7a16/P329] ORIG 0x084e43bc 段：reason/kind 均为寄存器右值（%edx/%eax），
        // 无具名槽 ⇒ 原码把两个三元内联在实参位（右到左求值序与 ORIG 一致）。
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x5e56,
            "Ignore SendBuffertoClient [%s][size:%d]; reason:%s, QQID:%s",
            (pPkt->m_kind == 0) ? "ANTIBOT" : "DPROTO", (int)(short)pPkt->m_len,
            (target == 0) ? "NULL User" : "NONE state",
            NumberToString(pPkt->m_uid, 0));
        if (target != 0 && (unsigned char)target->m_field4 < 5)
        {
            // [R7a16/P0] ORIG 0x084e45xx 段 old 无槽（`movzbl %al,%edx` + `lea 0x1(%eax),%ecx`
            // 直落寄存器）⇒ 原码是后缀自增的返回值直传实参位。
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)3, 0x252,
                (int)(unsigned char)target->m_field4++, 0);
        }
        return 0;
    }

    int packetType;                                    // 0x230=ANTIBOT / 0x231=DPROTO
    if (pPkt->m_kind == 0)
        packetType = 0x230;
    else
        packetType = 0x231;

    if (packetType == 0x230)
    {
        if (target->m_abnormalUserLogType == 2)
        {
            target->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)2);
            target->m_abnormalUserLogType = 3;
        }
        else if (target->m_abnormalUserLogType != 4)
        {
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b,
                (int)(short)pPkt->m_len,
                target->m_abnormalUserLogType);
        }
    }

    PacketGuard packet;
    packet.put_header(0, packetType);
    packet.put_binary(
        buffer, (int)(short)pPkt->m_len);
    packet.finalize(false);
    bool sendRet = target->Send(packet);

    if (packetType == 0x230)
    {
        if (target->m_abnormalUserLogType == 3)
        {
            target->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)3);
            target->m_abnormalUserLogType = 4;
        }
        else if (target->m_abnormalUserLogType != 4)
        {
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b,
                (int)(short)pPkt->m_len,
                target->m_abnormalUserLogType);
        }
    }
    else if (packetType == 0x231 &&
             (unsigned char)target->m_field4 < 5)
    {
        target->m_historyLog.SendBuffertoClientLog(
            pPkt->m_kind,
            (short)pPkt->m_len,
            (unsigned char)target->m_field4++, sendRet);
    }
    return 0;
}