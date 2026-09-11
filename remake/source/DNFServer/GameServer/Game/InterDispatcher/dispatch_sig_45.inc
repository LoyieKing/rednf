int Inter_MonitorNoticeGuildNames::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x14(%ebp)
    Inter_MonitorNoticeGuildNamesPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildNamesPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x255f,
            "Packet_Monitor_Notice_Guild_Char_Names::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)        // msg+0x12 公会 key 校验（ORIG 0x084cb083 mov 0x12(%edx)）
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2565,
            "Inter_MonitorCallGuildMembers::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x3f);
    // ORIG：count 缓存槽 -0x10(%ebp) int；循环 i<count 有符号 setl 尾测
    const int count =
        static_cast<int>(static_cast<unsigned int>(pPkt->m_16));
    packet.put_byte(count);
    for (int i = 0; i < count; ++i)
        // ORIG：数组寻址两步位移（每名 0x1e 定长，名起于元素 +7）。
        packet.put_str(pPkt->m_names[i].m_name, 0x1e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}