// [b28d-lane2] verdict=DIFF ndiff=129（ORIG 0x084d0de0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=74（ORIG 0x84d0de0）：P226 具名 bool/char 落槽+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_Monitor_RegisterBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_Monitor_RegisterBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_RegisterBlackListResultPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x79);

    if (pPkt->m_30 == '\x01')
    {
        if (!user->registerToBlackList(
                pPkt->m_2c))
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x2f90,
                "_BLACK_LIST_ :: false == pUser->registerToBlackList( recv->m_uCharacNo )");
            packet.put_byte(0);
            packet.put_int(0x4a);
        }
        else
        {
            packet.put_byte(1);
        }
    }
    else if (pPkt->m_30 == '\x02')
    {
        packet.put_byte(0);
        packet.put_int(0x4a);
    }
    else if (pPkt->m_30 == '\x03')
    {
        packet.put_byte(0);
        packet.put_int(0x4c);
    }
    else if (pPkt->m_30 == '\x04')
    {
        packet.put_byte(0);
        packet.put_int(0x4e);
    }
    else if (pPkt->m_30 == '\x05')
    {
        packet.put_byte(0);
        packet.put_int(0x5a);
    }

    const int local_10 = (int)strlen(pPkt->m_e);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_e, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}