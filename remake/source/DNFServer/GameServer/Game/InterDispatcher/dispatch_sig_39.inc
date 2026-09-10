#pragma pack(push, 1)
// P248 同型化：ORIG 084ca04b~084ca053 比较处发 4 行
//   `mov %eax,%edx; mov -0xc(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ 字段与 int 返回值失配 = unsigned int（c6root 4.4.7 实测：int 字段给 3 行形
//   `mov <slot>,%edx; mov 0xe(%edx),%edx; cmp %edx,%eax`，unsigned int 才给 ORIG 的 4 行形）。
//   聚合器 Inter_MonitorNoticeGuildDismissPktView.m_e 声明为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorNoticeGuildDismissPktViewI
{
    char m_pad0[0xe];               // +0x00..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildDismiss::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084ca02f：包基址在 get_state 检查之后物化入 -0xc(%ebp)
    Inter_MonitorNoticeGuildDismissPktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildDismissPktViewI*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2429,
            "Inter_MonitorNoticeGuildDismiss::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x242d, 0)("Guild Create Right Init! Char id : %d, Master Id : %d",
          user->get_charac_no(-1),
          reinterpret_cast<STGuildDBInfoPacketView*>(user->GetGuildDBInfo())->m_masterNo);

    // 会长本人额外清创建权（GmCmds_C.cpp 同型先例）
    if (reinterpret_cast<STGuildDBInfoPacketView*>(user->GetGuildDBInfo())->m_masterNo ==
        user->get_charac_no(-1))
    {
        user->set_guild_create_right(0);
    }
    user->set_charac_guildkey(0);

    PacketGuard packet;
    packet.put_header(0, 0x3d);
    packet.finalize(true);
    user->Send(packet);
    return 0;
