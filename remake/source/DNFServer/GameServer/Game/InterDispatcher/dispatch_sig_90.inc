// [b28d-lane2] verdict=DIFF ndiff=116（ORIG 0x084d1034）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=58（ORIG 0x84d1034）：P226 具名 bool/char 落槽+N18a 单段 regmove。
int Inter_Monitor_DeleteBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084d1042 段：包基址在 state 门之后（且晚于首个分支块）才物化
    //（`mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 落于 jmp 之后）⇒ 声明位置照抄（P0）。
    Inter_Monitor_DeleteBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_DeleteBlackListResultPktView*>(data);

    PacketGuard packet;
    packet.put_header(1, 0x7a);
    // ORIG 0x84d1034 段：无 result 具名槽（每比较点重发 `mov -0xSLOT(%ebp),%eax;
    // movzbl 0x30(%eax),%eax; cmp $K,%al`）⇒ 内联 pPkt->m_30（P0/P226）。
    if (pPkt->m_30 == 1)
    {
        // deleteToBlackList 返回 bool：ORIG xor $0x1,%eax; test %al 取反检验
        if (!user->deleteToBlackList(
                pPkt->m_2c))
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x2fd7,
                "_BLACK_LIST_ :: false == pUser->deleteToBlackList( recv->m_uCharacNo )");
            packet.put_byte(0);
            packet.put_int(0x4b);
        }
        else
        {
            packet.put_byte(1);
        }
    }
    else if (pPkt->m_30 == 2)
    {
        packet.put_byte(0);
        packet.put_int(0x4b);
    }
    else if (pPkt->m_30 == 3)
    {
        packet.put_byte(0);
        packet.put_int(0x4c);
    }

    const int nameLen =                              // msg+0x0e 目标名
        static_cast<int>(strlen(pPkt->m_e));
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_e, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}