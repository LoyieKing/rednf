int Inter_MonitorCallGuildAllMembers::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorCallGuildAllMembersPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildAllMembersPktView*>(data);

    // ORIG：同 MonitorCallGuildMembers。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x250c,
            "Inter_MonitorCallGuildAllMembers::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2512,
            "Inter_MonitorCallGuildAllMembers::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x8f);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);
    // ORIG 0x084cacxx `mov -0x10(%ebp),%eax; lea 0x16(%eax),%edx` 先于 `mov -0xc(%ebp),%eax`
    // ⇒ put_str 实参1（串址）先求值；strlen 结果 int 槽直传（size_t+(int)cast 形反序退化 add）。
    const int msgLen = strlen(pPkt->m_16);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_16, msgLen);
    packet.put_int(pPkt->m_2d);
    packet.put_short(
        (int)(unsigned short)pPkt->m_31);
    FunctionGuildMember::MakePacketGuildMembersInfo(
        packet,
        // ORIG 0x084cadxx：arg3 `mov -0x10(%ebp),%eax; lea 0x34(%eax),%edx`（槽寻址、先于
        // arg2 的 movzbl 0x33）⇒ 基址改用 pPkt 槽而非 data 形参。
        (int)pPkt->m_33,
        reinterpret_cast<ST_Guild_Mem_Info*>(reinterpret_cast<char*>(pPkt) + 0x34));
    packet.finalize(true);
    user->Send(packet);
    return 0;
