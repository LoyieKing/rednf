int Timer_HadesHeartBeat::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_hades_proxy->GetSocketState() == 0) {
        // ORIG 0x0863a760：channelNo 直落 %ebx（push %ebx 帧）、udp_port 参数位
        // movzwl %ax 免槽（探针 x7 实证 register + 实参内联 cast 形态）。
        register int channelNo = G_CEnvironment()->get_channel_no();
        GlobalData::s_hades_proxy->SendHeartBeatPacket(
            (unsigned short)G_CEnvironment()->get_udp_port(), channelNo);
        registNextTimer();
    }
    return 1;
