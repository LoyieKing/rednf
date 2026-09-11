// ---------------------------------------------------------------------------
// ST_Guild_War_Rank_Info / Packet_Notice_Guild_War_Info 生命周期（StubS3C）。
// ST ctor：ORIG W 0x0822ba9a —— uint(+0)=0、uint(+4)=0、ushort(+8)=0、
// memset(+0xa,0,0x17)。Packet_Notice ctor：ORIG W 0x0822badc ——
// PacketHeader(0x414,0x15c) 后对 +0x12 起 9 个元素（stride 0x21）逐个构造。
// ---------------------------------------------------------------------------
ST_Guild_War_Rank_Info::ST_Guild_War_Rank_Info()

{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    memset(m_pad0a, 0, sizeof(m_pad0a));
}
