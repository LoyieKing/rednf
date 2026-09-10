int Inter_GuildCargoUpgrade::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoUpgradePktView* pPkt = reinterpret_cast<Inter_GuildCargoUpgradePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // P0：ORIG 无该局部槽（`mov %eax,%edx` 表达式直比）⇒ 内联，禁具名缓存。
    // ORIG `cmp %eax,%edx`（dst=call 结果）⇒ 书写序以 callee 调用为左操作数。
    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x10d);
    // ORIG `je <then>`（then 块后置、else 直落）⇒ P272 反条件块序交换。
    if (pPkt->m_12 != 0xc1)
    {
        packet.put_byte(0);
        packet.put_byte((unsigned int)pPkt->m_12);
    }
    else
        packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
