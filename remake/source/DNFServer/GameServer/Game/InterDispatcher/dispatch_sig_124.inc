int Inter_SecurityCardIssue::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x1c(%ebp)
    Inter_SecurityCardIssuePktView* pPkt = reinterpret_cast<Inter_SecurityCardIssuePktView*>(data);
    if (pPkt->m_8c == 0)
    {
        char encSerial[0x42] = { '\0' };               // 密文 serial
        char idStr[0x30] = { '\0' };                   // 账号串
        char encAnswers[0x42];                         // 密文 answers（显式 memset）

        OS_API::snprintf(idStr, 0x30, "%s",
                         NumberToString((unsigned int)user->get_acc_id(), 0));
        GlobalData::s_securityCardCenter->encryptString(
            reinterpret_cast<char*>(pPkt) + 4, encSerial);   // ORIG 经 -0x1c 槽
        GlobalData::s_securityCardCenter->encryptString(idStr, idStr);
        memset(encAnswers, 0, 0x42);
        GlobalData::s_securityCardCenter->encryptString(
            reinterpret_cast<char*>(pPkt) + 0x25, encAnswers);  // ORIG 经 -0x1c 槽

        PacketGuard packet;
        packet.put_header(1, 0xa4);
        packet.put_byte(1);
        packet.put_int(strlen(idStr));
        packet.put_str(idStr, strlen(idStr));
        packet.put_int(strlen(encSerial));
        packet.put_str(encSerial, strlen(encSerial));
        packet.put_int(strlen(encAnswers));
        packet.put_str(encAnswers, strlen(encAnswers));
        packet.finalize(true);
        user->Send(packet);

        user->getSecurityCard()->init();
        user->getSecurityCard()->setSecurityCardInfo(
            reinterpret_cast<char*>(pPkt) + 0x37, false, false, 0, 0, 0,
            reinterpret_cast<char*>(pPkt) + 0x5e);    // ORIG 经 -0x1c 槽
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4,
                                 (unsigned char)pPkt->m_8c);
    }
    return 0;
