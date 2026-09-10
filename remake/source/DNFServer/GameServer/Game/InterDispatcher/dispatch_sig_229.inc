int Inter_PvPBuddyConnList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b28c-30] N22 封顶：ORIG 0x084e1709 `cmp %eax,%ebx`（ebx = 局部字段值）vs
    // 4.4 `cmp %ebx,%eax`（call 结果在 %eax）；cmp 两向不可翻（dst=局部 vs
    // dst=call 结果）。
    Inter_PvPBuddyConnListPktView* pPkt = reinterpret_cast<Inter_PvPBuddyConnListPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG 一步 `mov 0xa(%eax),%ebx` 直落 callee-saved（具名槽会顶深 pPkt，技巧2）。
    register int charIdMsg =                           // recv charId @ msg+0xa
        pPkt->m_a;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    // ORIG：m_12 每处独立 reload（无 count 槽；put_int + 循环界各一次）。
    PacketGuard packet;
    packet.put_header(0, 0x10f);
    packet.put_int((int)pPkt->m_12);
    for (int i = 0; i < (int)pPkt->m_12; ++i)
        // 求值序 = ORIG：i 项先算（eax），再 add $0x10 / add pPkt 槽 / add $0x6
        // （pPkt 置于两常量之间阻折叠，4.4 恒发三段 add；旧形 pPkt 先置产 ecx 提前载+lea）。
        // 形探针实测：指针形/收窄括号形 ndiff 反劣（8→12/>12）⇒ 保留原「pPkt 置两常量之间」形
        //（ndiff 8，残 = 4.4 lea 合成 vs ORIG 三段 add，P30/N4 封顶）。
        packet.put_binary(reinterpret_cast<char*>(i * 0x29 + 0x10 +
                                                  reinterpret_cast<int>(pPkt) + 6),
                          0x29);
    packet.finalize(true);
    user->Send(packet);
    return 0;
