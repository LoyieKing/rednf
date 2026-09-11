void CAssaultMgr::SetAssaultChannel(bool flag)
{
    // ORIG W _ZN11pvp_assault11CAssaultMgr17SetAssaultChannelEb @0x08376048：
    // movzbl flag → *(unsigned char*)this = flag（写入 +0x00 通道开关）。
    m_bAssaultChannel = flag;
}