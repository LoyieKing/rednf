// [b28d-lane2] verdict=DIFF ndiff=130（ORIG 0x084e1978）：寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=41（ORIG 0x84e1978）：P231 register int 局部
// 未能翻单段（4.4 仍两段）；余 m_32 分支 je/jne 极性 + N4 add-vs-lea。
int Inter_AddPvPBuddyResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AddPvPBuddyResultPktView* pPkt = reinterpret_cast<Inter_AddPvPBuddyResultPktView*>(data);
    if (user->get_state() <= 2)
        return 0;
    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    if (pPkt->m_12 == '\0')
    {   // 拒绝分支（0x121）
        // P243 书写序：ORIG 0x084e1980 `test %al,%al; je <put121>` ⇒ err 块 fall-through 在前
        //   （原码为 `if (m_32 != 0) { err } else { put }`），非我方旧稿的 `== '\0'` 先 put。
        if (pPkt->m_32 != '\0')
        {
            user->SendCmdErrorPacket(
                (ENUM_CMDPACKET)0x121,
                pPkt->m_32);
        }
        else
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(1, 0x121);
            packet.put_byte(1);
            packet.put_byte(pPkt->m_13);
            const int local_14 = (int)strlen(pPkt->m_14);
            packet.put_int(local_14);
            packet.put_str(pPkt->m_14, local_14);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    else if (pPkt->m_32 != '\0')
    {   // 拒绝分支（0x122）：同上 ORIG 0x084e19e0 `test %al,%al; je <put122>` ⇒ err 先
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x122,
            pPkt->m_32);
    }
    else
    {   // 同意分支（0x122）
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x122);
        packet.put_byte(1);
        packet.put_byte(pPkt->m_13);
        // [L9·2026-09-11 子代理 B-PATCH#2] size_t→int（P265 同型化；同函数 case-0 分支已是 int）：
        // ORIG 084e1ba8/084e1bab/084e1bae `mov -0x14(%ebp),%eax; lea 0x14(%eax),%edx; mov -0xc(%ebp),%eax`
        // （先算 ptr 再装 len）；探针 /tmp/L9/B/pb_str.cpp b1(int)→lea 形逐条同形。
        const int local_10 = (int)strlen(pPkt->m_14);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_14, local_10);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}