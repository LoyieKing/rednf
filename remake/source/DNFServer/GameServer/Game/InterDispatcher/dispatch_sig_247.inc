// [b28d-lane2] verdict=DIFF ndiff=107（ORIG 0x084e2a06）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=57（ORIG 0x84e2a06）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_ReqQueryCharacInfoMailbox::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    char* recv = data;                                 // -0xc(%ebp) 缓存消息指针

    if (reinterpret_cast<SIG_QUERY_CHARAC_INFO*>(recv)->m_errCode != 0)
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x145,
            (unsigned char)reinterpret_cast<SIG_QUERY_CHARAC_INFO*>(recv)->m_errCode);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x145);
    packet.put_byte(1);
    packet.put_int(strlen(recv));
    packet.put_str(recv, strlen(recv));                // ORIG 两次独立 strlen（P48）
    packet.put_short((int)reinterpret_cast<SIG_QUERY_CHARAC_INFO*>(recv)->m_lev);  // movzwl+cwtl
    packet.put_byte((int)reinterpret_cast<SIG_QUERY_CHARAC_INFO*>(recv)->m_job);
    packet.put_byte((int)reinterpret_cast<SIG_QUERY_CHARAC_INFO*>(recv)->m_growType);
    packet.put_byte((int)reinterpret_cast<SIG_QUERY_CHARAC_INFO*>(recv)->m_sex);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}