#pragma pack(push, 1)
// [G2A-PC 手法1/P248] ORIG 084d4942 对 guildDbInfo+0x1b 发单段 `movzbl 0x1b(%eax),%eax`；
//   我方旧写 `*(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b)` 多发
//   `add $0x1b,%eax` 一条 ⇒ 原码是视图成员访问（pack(1) 常量偏移）。
struct Inter_ReplyJoinPowerGuildDbByteViewI
{
    char m_pad0[0x1b];
    unsigned char m_c;                              // +0x1b (ORIG 084d4942 movzbl 单段 = unsigned char)
};
#pragma pack(pop)

int Inter_ReplyJoinPower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReplyJoinPowerPktView* pPkt = reinterpret_cast<Inter_ReplyJoinPowerPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] ORIG 084d47f4 段：比较为内联形（`call get_charac_no; mov -0x10(%ebp),%eax;
    // mov 0x12(%eax),%eax; cmp %eax,%edx`），logFormat 两处亦逐点重读 ⇒ 原码无 characNo
    // 具名局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_12)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x35fc,
            "Inter_ReplyJoinPower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_12);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb4);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(
            (int)pPkt->m_16);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, msg+0x16)
        const char* guildDbInfo =
            static_cast<const char*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo,
            // P248 同型化：ORIG 084d4942 `movzbl 0x1b(%eax),%eax` 单段 disp 读。
            ((const Inter_ReplyJoinPowerGuildDbByteViewI*)guildDbInfo)->m_c,
            pPkt->m_16);
        // [G2A-PC 手法2/P324] ORIG 084d4969 `call get_charac_guildkey` → 084d496e
        //   `mov %eax,%ebx`（跨两调用保活，无栈槽）→ 084d4976/084d4988 receiver 链
        //   （GetServerGroup → GetServerProxy）→ 084d499b call ModifyGuildInfoProxy：
        //   先算实参、后算 receiver ⇒ 原码无具名 proxySrv 局部（具名形多 store+load 往返）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
