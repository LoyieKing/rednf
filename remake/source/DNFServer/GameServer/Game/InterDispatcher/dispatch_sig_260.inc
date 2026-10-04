// [b28d-lane2] verdict=DIFF ndiff=110（ORIG 0x084e3924）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=66（ORIG 0x84e3924）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_RequestJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    Inter_RequestJoinGuildPktView* pPkt = reinterpret_cast<Inter_RequestJoinGuildPktView*>(data);

    if (pPkt->m_errCode != 0)          // msg+0x00 错误码
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x15d,
            static_cast<unsigned char>(
                pPkt->m_errCode & 0xff));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x15d);
    packet.put_byte(1);
    packet.put_int(pPkt->m_4);
    // 2026-09-11 L9GrpM（P0/P183 具名量合并）：ORIG Ghidra 段 `local_10 = strlen(msg+0x26);
    //   …; local_10 = strlen(msg+8);` = **同一局部两次赋值**（单槽 -0x10）；我方两具名
    //   `guildNameLen`/`requesterLen` 各占一槽 ⇒ pPkt 由 ORIG 的 -0x10 被挤到 -0x14。
    //   同文件 dispatch_sig_259（Inter_CheckJoinGuild）已是 `int strLen` 复用形（先例）。
    int strLen = static_cast<int>(strlen(pPkt->m_guildName));   // msg+0x26 公会名
    packet.put_int(strLen);
    packet.put_str(pPkt->m_guildName, strLen);
    strLen = static_cast<int>(strlen(pPkt->m_requesterName));        // msg+0x08 申请者名
    packet.put_int(strLen);
    packet.put_str(pPkt->m_requesterName, strLen);
    packet.put_int(pPkt->m_40);
    packet.put_int(pPkt->m_44);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}