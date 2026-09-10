int Inter_Monitor_RequestBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_Monitor_RequestBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_RequestBlackListResultPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x7b);
    packet.put_byte(1);
    if (pPkt->m_e <= 0xa)
    {
        packet.put_byte(
            (int)pPkt->m_e);
        for (int i = 0;
             i < (int)(unsigned int)pPkt->m_e;
             ++i)
        {
            const int nameLen = (int)strlen(data + i * 0x28 + 0x13);
            if (nameLen == 0)
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x2f6d, "BlackListNameLen Err charno(%d)",
                    user->getCurCharacNo());
            }
            packet.put_int((int)nameLen);
            packet.put_str(data + i * 0x28 + 0x13, (int)nameLen);
            struct tm tmVal;                               // P43：经返回值访问
            struct tm* ptm = localtime_r(
                reinterpret_cast<time_t*>(data + i * 0x28 + 0x33), &tmVal);
            packet.put_byte(ptm->tm_year);
            packet.put_byte(ptm->tm_mon);
            packet.put_byte(ptm->tm_mday);
            user->registerToBlackList(
                *reinterpret_cast<unsigned int*>(data + i * 0x28 + 0xf));
        }
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
