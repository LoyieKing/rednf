// [b28d-lane2] verdict=DIFF ndiff=147（ORIG 0x084d2b96）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=29（ORIG 0x84d2b96）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_CallGuildInviteToInvited::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084d2bbc：state 岛后 pPkt 才物化 -0x14（技巧13）。
    Inter_CallGuildInviteToInvitedPktView* pPkt = reinterpret_cast<Inter_CallGuildInviteToInvitedPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x337b,
            "MEMBER : Inter_CallGuildInviteToInvited::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    if (user->alonePlay_Mode())
    {
        // [G2A-PC P0 反向] ORIG 0x084d2c4c 段为 `movb $0x2,-0x9(%ebp)` +
        //   `movzbl -0x9(%ebp),%esi`：mode 值**经栈槽**取出 ⇒ 原码是普通具名局部。
        //   本 diff N 侧 `mov $0x2,%esi` + `and $0xff,%esi` 正是 `register` 免槽
        //   直落形（反了）⇒ 去掉 register 恢复落槽。characNo 侧 ORIG 直落 %ebx
        //   （跨链式 call 保活、无槽），故其 register 保留。
        unsigned char modeE = 2;
        register unsigned int characNo = user->getCurCharacNo();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendReplyGuildInvite(characNo, modeE);
        return 0;
    }

    if (user->GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1e) != 0)
    {
        // 同族第二份（ORIG 0x084d2cbd `movb $0x3,-0x9(%ebp)` + `movzbl -0x9(%ebp),%esi`）。
        unsigned char modeD = 3;
        register unsigned int characNo = user->getCurCharacNo();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendReplyGuildInvite(characNo, modeD);
    }

    PacketGuard packet;
    packet.put_header(0, 0x93);
    const int local_14 = (int)strlen(pPkt->m_guildName);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_guildName, local_14);
    const int local_10 = (int)strlen(pPkt->m_name);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_name, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}