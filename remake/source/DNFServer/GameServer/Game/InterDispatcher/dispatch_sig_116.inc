// [b28d-lane2] verdict=DIFF ndiff=132（ORIG 0x084c9768）：寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=24（ORIG 0x84c9768）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_ReplyGuildSecede::dispatch_sig(CUser* user, char* data, int len)
{
    // ORIG 无 len 槽（Ghidra local_28 系 PG 内部视图误标）；pPkt 入口即物化 -0x24。
    if (user->get_state() == 0)
        return 0;

    Inter_ReplyGuildSecedePktView* pPkt = reinterpret_cast<Inter_ReplyGuildSecedePktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x236b,
            "Inter_ReplyGuildSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9c);
    if (pPkt->m_failReason != 0)
    {   // 退会失败（原因码）
        packet.put_byte(0);
        packet.put_byte(pPkt->m_failReason);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {   // 退会成功
        packet.put_byte(1);
        packet.put_byte(pPkt->m_secedeFlag);
        const int local_24 = (int)strlen(pPkt->m_guildName);
        packet.put_int(local_24);
        packet.put_str(pPkt->m_guildName, local_24);
        const int local_20 = (int)strlen(pPkt->m_characName);
        packet.put_int(local_20);
        packet.put_str(pPkt->m_characName, local_20);
        packet.finalize(true);
        user->Send(packet);
        user->m_historyLog.GuildOut(
            pPkt->m_guildKey,
            // [G2A-PC 手法1] ORIG 084c997e `mov -0x24(%ebp),%eax` → 084c9981
            //   `lea 0x3b(%eax),%edi`：-0x24 即 pPkt 槽 ⇒ 原码基址用 pPkt 具名指针，
            //   非形参 data（形参直读会发 `mov 0x10(%ebp),%eax`）。
            pPkt->m_guildName,
            pPkt->m_characNo,
            pPkt->m_secedeFlag,
            static_cast<unsigned short>(
                pPkt->m_totalCnt - 1));
    }

    user->getCurCharacInvenR()->SendItemLockListInven();
    user->getCurCharacInvenR()->SendItemLockListAvatar();
    user->getCurCharacCargoR()->SendItemLockList();
    user->getCurCharacInvenR()->SendItemLockListCreature();
    return 0;
}