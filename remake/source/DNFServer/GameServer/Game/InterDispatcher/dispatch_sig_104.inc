int Inter_GMRequestMid::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // b28c-28（P29/P183 复验）：ORIG 0x084d27c4 段序为「早退岛之后」才
    // `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`（pPkt 初始化在 get_state
    // 判定之后），且 strlen 结果有**槽往返**
    // （0x084d283x：`call strlen; mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax`
    // → 后两点各 `mov -0xc(%ebp),%eax` 回读）⇒ 原码是具名长度局部、
    // strlen 只求值一次（P0：有往返即有具名局部）；旧「前置 dead gapPad
    // 推槽 + 两次 strlen」实为错位建模（多 1 条 call strlen + pPkt 槽差）。
    Inter_GMRequestMidPktView* pPkt = reinterpret_cast<Inter_GMRequestMidPktView*>(data);

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x7e);
    packet.put_int(pPkt->m_e);
    packet.put_int(pPkt->m_12);
    int nameLen = strlen(pPkt->m_16);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_16, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
