int Dispatcher_PvpMissionHPPercent::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PVP_MISSION_HP_PERCENT& data = reinterpret_cast<MSG_PVP_MISSION_HP_PERCENT&>(msg);
    packet.get_byte(data.m_0d);
    if (data.m_0d == 0 || data.m_0d > 4)
    {
        // b28c-33：清 P0 存量绑定 `register asm("ebx")`；按 P231 纯 register 存储类
        // 提示重试自然形（ORIG 该值 movzbl 直落 %ebx 跨 ctor 保活、无栈槽）。
        register unsigned int cnt = data.m_0d;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3924, 5);
        trace("[@missionSystem] HackUser: <clearMissionCount : %d>", cnt);
        return LineFunc(0x3925,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    data.m_hpPercents.resize(static_cast<size_t>(data.m_0d), 0);
    for (int i = 0; i < static_cast<int>(data.m_0d); ++i)
    {
        packet.get_short(data.m_hpPercents[i]);
    }
    return 0;
}