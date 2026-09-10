int Inter_ReplyGuildSecede::dispatch_sig(CUser* user, char* data, int len)
{
    // ORIG 无 len 槽（Ghidra local_28 系 PG 内部视图误标）；pPkt 入口即物化 -0x24。
    if (user->get_state() == 0)
        return 0;

    Inter_ReplyGuildSecedePktView* pPkt = reinterpret_cast<Inter_ReplyGuildSecedePktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_10)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x236b,
            "Inter_ReplyGuildSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_10);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9c);
    if (pPkt->m_18 != 0)
    {   // 退会失败（原因码）
        packet.put_byte(0);
        packet.put_byte(pPkt->m_18);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {   // 退会成功
        packet.put_byte(1);
        packet.put_byte(pPkt->m_1c);
        const int local_24 = (int)strlen(pPkt->m_3b);
        packet.put_int(local_24);
        packet.put_str(pPkt->m_3b, local_24);
        const int local_20 = (int)strlen(pPkt->m_1d);
        packet.put_int(local_20);
        packet.put_str(pPkt->m_1d, local_20);
        packet.finalize(true);
        user->Send(packet);
        user->m_historyLog.GuildOut(
            pPkt->m_a,
            // [G2A-PC 手法1] ORIG 084c997e `mov -0x24(%ebp),%eax` → 084c9981
            //   `lea 0x3b(%eax),%edi`：-0x24 即 pPkt 槽 ⇒ 原码基址用 pPkt 具名指针，
            //   非形参 data（形参直读会发 `mov 0x10(%ebp),%eax`）。
            reinterpret_cast<char*>(pPkt) + 0x3b,
            pPkt->m_10,
            pPkt->m_1c,
            static_cast<unsigned short>(
                pPkt->m_e - 1));
    }

    user->getCurCharacInvenR()->SendItemLockListInven();
    user->getCurCharacInvenR()->SendItemLockListAvatar();
    user->getCurCharacCargoR()->SendItemLockList();
    user->getCurCharacInvenR()->SendItemLockListCreature();
    return 0;
