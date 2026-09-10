int Inter_ResposncePCRoomPlyerList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N（本轮未改）：151 行残差为 (a) 常量拆合差（ORIG `add $0x10,%eax;
    //   add <S>,%eax; add $0x7,%eax` vs 我方 `add $0x17,%eax; lea (%ecx,%eax,1),%eax`；
    //   前者暗示两级成员偏移，但无视图/头依据不宜臆造）；(b) 步长展开重排
    //   （ORIG `shl $0x3; add %edx,%eax; shl $0x2; ...`）；(c) `mov <S>,%ecx` 中转。留 N。
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    PacketGuard packet;
    if (user->GetPCRoomNo() == 0)                      // 非网吧用户回执拒绝
    {
        packet.put_header(1, 0xd8);
        packet.put_byte(0);
        packet.put_byte(3);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    Inter_ResposncePCRoomPlyerListPktView* pPkt = reinterpret_cast<Inter_ResposncePCRoomPlyerListPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x4192,
            "Inter_ResposncePCRoomPlyerList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    if (pPkt->m_12 > 0x28)   // 条目数越界
    {
        packet.put_header(1, 0xd8);
        packet.put_byte(0);
        packet.put_byte(3);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    packet.put_header(1, 0xd8);
    packet.put_byte(1);
    packet.put_byte((int)pPkt->m_12);

    // ORIG 0x84d9ed4/0x84d9f09 等 7 处 `add -0x14(%ebp),%eax`：循环体基址走 pPkt 具名槽。
