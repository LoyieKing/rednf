#pragma pack(push, 1)
// [G2A-PC 手法1/P248] ORIG 084d4b3a 对 guildDbInfo+0x1b 发单段 `movzbl 0x1b(%eax),%eax`
//   （结构体成员 = base+disp 形）；我方旧写 `*(reinterpret_cast<const unsigned char*>
//   (guildDbInfo) + 0x1b)` 会多发 `add $0x1b,%eax` 一条 ⇒ 原码是视图成员访问。
//   pack(1) 布局与聚合器无关（本件自带，只覆盖 +0x1b 一字节）。
struct Inter_SecedePowerGuildDbByteViewI
{
    char m_pad0[0x1b];
    unsigned char m_c;                              // +0x1b (P248: ORIG 084d4b3a movzbl 单段 = unsigned char)
};
#pragma pack(pop)

int Inter_SecedePower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SecedePowerPktView* pPkt = reinterpret_cast<Inter_SecedePowerPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] ORIG 084d49f6 段：比较内联 + logFormat 逐点重读 ⇒ 原码无 characNo 具名
    // 局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_12)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3620,
            "Inter_SecedePower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_12);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb5);
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

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, 0)
        // ORIG 0x84d4b2f-0x84d4b41：单次调用 + `mov %eax,-0xc` 槽往返 ⇒ 具名局部（p167 E=GetGuildDBInfo×1）。
        const char* guildDbInfo = static_cast<const char*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo,
            // P248 同型化：ORIG 084d4b3a `movzbl 0x1b(%eax),%eax` 单段 disp 读。
            ((const Inter_SecedePowerGuildDbByteViewI*)guildDbInfo)->m_c,
            (unsigned char)0);
        // [G2A-PC 手法2/P324] ORIG 084d4b65 `call get_charac_guildkey` → 084d4b6a
        //   `mov %eax,%ebx`（跨两调用保活，无栈槽）→ 084d4b6c/084d4b72 `call GetServerGroup`
        //   → 084d4b84 `call GetServerProxy`：即先算实参、后算 receiver 链。
        //   具名 proxySrv 局部会多发 `mov %eax,-0xNN(%ebp)` + `mov -0xNN(%ebp),%eax`
        //   往返（本 diff 的 N 侧尾部即此形）⇒ 原码 receiver 为链式单表达式。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
