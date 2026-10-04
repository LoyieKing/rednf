// [b28d-lane2] verdict=DIFF ndiff=263（ORIG 0x084dee6a）：槽序/帧差(N82/P212/P221)+混合残差；gap+19。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=221（ORIG 0x84dee6a）：N22 cmp 操作数方向+N101 imul/lea 折叠+N18a 单段 regmove。
int Inter_GuildBoardData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildBoardDataPktView* pPkt = reinterpret_cast<Inter_GuildBoardDataPktView*>(data);
    // ORIG 0x84dee8e/0x84deebc/0x84df274 三处 `mov $0x0,%ebx; jmp <epilogue>`（统一出口
    // `mov %ebx,%eax`）⇒ 原码为三个独立 `return 0`（P16 early-return 形，非单一出口）。
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;
    if (user->get_charac_no(-1) !=
        pPkt->m_characNo)
        return 0;

    PacketGuard packet;                                // -0x44

    // §P0/P251：ORIG 对 msg+0xc 无槽往返（比较与 cMyTrace 实参各自重读 movzwl 双段）⇒ 不缓存 code。
    // ORIG 0x84deed4 为 `movzwl; movzwl %ax,%eax; cmp $0x232a,%eax`，4.4 恒折 `cmp $0x232a,%ax`
    // （显式 (unsigned int) 提升实测无效，已回退，P162/N7 封顶）。
    if (pPkt->m_boardType == 0x232a)
        packet.put_header(1, 0x155);
    else if (pPkt->m_boardType == 0x232e)
        packet.put_header(1, 0x156);

    if (pPkt->m_result == 0)
    {   // 正常数据分支
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_e);
        packet.put_byte((int)pPkt->m_rowCount);
        // ORIG 全程以 -0x28 具名槽（pInfo）作基址（084df088 `add -0x28(%ebp),%edx` 等 9 处）
        // ⇒ 循环体基址走 pPkt 槽，不用形参 data。
        for (int i = 0;
             i < (int)pPkt->m_rowCount; ++i)
        {
            char flag = 0;
            if (pPkt->m_rows[i].m_flag == 1)
                flag = 1;
            packet.put_byte((int)(unsigned char)flag);
            packet.put_byte((int)pPkt->m_rows[i].m_kind);
            if (user->get_charac_no(-1) == pPkt->m_rows[i].m_characNo)
                packet.put_byte(1);
            else
                packet.put_byte(0);

            packet.put_int((int)strlen(pPkt->m_rows[i].m_text));
            packet.put_str(pPkt->m_rows[i].m_text,
                           (int)strlen(pPkt->m_rows[i].m_text));
            packet.put_int(pPkt->m_rows[i].m_id);

            int remainSec =
                GlobalData::s_systemTime_.getCurSec() -
                pPkt->m_rows[i].m_time;
            if (remainSec < 0)
                remainSec = 0;
            packet.put_int(remainSec);

            packet.put_int((int)strlen(pPkt->m_rows[i].m_title));
            packet.put_str(pPkt->m_rows[i].m_title,
                           (int)strlen(pPkt->m_rows[i].m_title));
        }

        // P328⑩/P227：ORIG 0x84df1a2..0x84df20a 先求 4 个实参（m_17→-0x4c、m_e→%edi、
        // m_c→%esi、get_charac_no→%ebx），其后才 cMyTrace ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x4b51, 0)(
            "GUILD_BOARD : CHARAC NO : %u, CODE : %u, TOTAL CNT : %u, CNT : %u : BOARD DATA SEND!!",
            user->get_charac_no(-1), (unsigned int)pPkt->m_boardType,
            (unsigned int)pPkt->m_e,
            (int)pPkt->m_rowCount);
    }
    else
    {   // 错误码分支
        packet.put_byte(0);
        packet.put_short((int)(unsigned int)
                         pPkt->m_result);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}