int Inter_CallGuildInviteToInvited::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084d2bbc：state 岛后 pPkt 才物化 -0x14（技巧13）。
    Inter_CallGuildInviteToInvitedPktView* pPkt = reinterpret_cast<Inter_CallGuildInviteToInvitedPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x337b,
            "MEMBER : Inter_CallGuildInviteToInvited::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    if (user->alonePlay_Mode())
    {
        // b28c-l23 P0 清绑：ORIG 0x084d2c4c 把 mode 常量直落 %esi、characNo 直落
        // %ebx（跨链式 call 保活）；绑定违反 P0 已删，改普通具名局部（4.4 落槽
        // ⇒ N96/N18a，P168a④ 判词），语义不变。[b28c-l23 修正] 仅删 asm("esi"/"ebx")
        // 绑定，裸 register 保留（免槽直落形）。
        register unsigned char modeE = 2;
        register unsigned int characNo = user->getCurCharacNo();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendReplyGuildInvite(characNo, modeE);
        return 0;
    }

    if (user->GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1e) != 0)
    {
        // b28c-l23 P0 清绑：同族第二份（ORIG 0x084d2cbd，slot -0x9）。
        register unsigned char modeD = 3;
        register unsigned int characNo = user->getCurCharacNo();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendReplyGuildInvite(characNo, modeD);
    }

    PacketGuard packet;
    packet.put_header(0, 0x93);
    const int local_14 = (int)strlen(pPkt->m_30);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_30, local_14);
    const int local_10 = (int)strlen(pPkt->m_12);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_12, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
