int Inter_ApplyTempPowerSide::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ApplyTempPowerSidePktView* pPkt = reinterpret_cast<Inter_ApplyTempPowerSidePktView*>(data);
    // [b28d-lane2] ORIG 084dccec 段为「字段先读」序（`mov -0x10(%ebp),%eax;
    // mov 0xe(%eax),%ebx; movl $0xffffffff,0x4(%esp); call get_charac_no; cmp %eax,%ebx`）
    // ⇒ 原码字段在比较左（`pPkt->m_characNo != user->get_charac_no(-1)`）；旧 call-first 序
    //   产 `mov %eax,%edx; mov 0xe(%eax),%eax; cmp %eax,%edx`（+3 条）。
    if (pPkt->m_characNo != user->get_charac_no(-1))
        return 0;

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, msg+0x16)
        // ORIG 单次调用 + 槽往返 ⇒ 具名局部（p167 E=GetGuildDBInfo×1）。
        const STGuildDBInfoOnly* guildDbInfo =
            static_cast<const STGuildDBInfoOnly*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo->m_guildName,
            guildDbInfo->m_fundLevel,
            pPkt->m_16);
        // [G2A-PC 手法2/P324] ORIG 084dcd33 后段：get_charac_guildkey 结果入 callee-saved
        //   （无栈槽）→ 084dcd66/084dcd79 receiver 链（GetServerGroup → GetServerProxy）
        //   → 084dcd8c call ModifyGuildInfoProxy：先算实参、后算 receiver ⇒ 原码无具名
        //   proxySrv 局部（具名形多 store+load 往返）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(), proxy);
    }

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    PacketGuard reply;
    reply.put_header(1, 0xc3);
    reply.put_byte(1);
    reply.put_byte(1);
    reply.finalize(true);
    user->Send(reply);
    return 0;
}