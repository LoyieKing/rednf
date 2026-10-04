// [b28d-lane2] verdict=DIFF ndiff=153（ORIG 0x084ce7f6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=125（ORIG 0x84ce7f6）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
// [b28d-SweepInterDispatcher2] verdict=AE（ORIG 0x084ce7f6，192=192，TU 121/1330/5/134）：真修四处——
//   ① ItemView(+0x12 起 stride 0x21，字段 +0/+4/+8/+0xa) 复现 ORIG `lea (%ecx,%eax,1); add $0x10; mov 0x2(%eax)` 两步寻址；
//   ② 循环 1 负条件 goto 尾空标签复现 4.1 nop 岛（P223）；③ pPkt 声明移到 GetChannelType 检查后（P0/声明位置）；
//   ④ 删死局部 local_20（P171）+ m_e int→unsigned int 触发比较点 `mov %eax,%edx` 中转（P211/P265 档③，sizeof/偏移/mangled 零影响）。
int Inter_MonitorGuildWarInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (G_GameWorld()->GetChannelType() != 6)
        return 0;

    Inter_MonitorGuildWarInfoPktView* pPkt = reinterpret_cast<Inter_MonitorGuildWarInfoPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2ad4,
            "GuildWarInfo : Inter_MonitorGuildWarInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x70);
    packet.put_byte(1);

    int local_1c = 0;
    for (int local_18 = 0; local_18 < 10; ++local_18)
    {
        if (pPkt->m_items[local_18].m_12 == 0)
            goto count_done;
        local_1c = local_1c + 1;
    count_done:;
    }
    packet.put_byte(local_1c);

    for (int local_14 = 0; local_14 < local_1c; ++local_14)
    {
        packet.put_int(pPkt->m_items[local_14].m_12);
        packet.put_int(pPkt->m_items[local_14].m_16);
        packet.put_short((unsigned int)pPkt->m_items[local_14].m_1a);
        const int local_10 = (int)strlen(pPkt->m_items[local_14].m_1c);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_items[local_14].m_1c, local_10);
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
}