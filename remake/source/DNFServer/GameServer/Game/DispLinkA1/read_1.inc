// ============================================================================
// 7. Dispatcher_Client_Spec_Statistic::read  ORIG 0x081ce5fe
//    msg+0xd/+0xe/+0xf 三字节，+0xf 为条数；条目步长 0xc：
//    +0x12 byte、+0x16 dword、+0x1a dword（ORIG 裸偏移直读 MSG 体）。
//    b28c-34 判词（identical）：ORIG 寻址为 (p + i*0xc) + K（先 add 基址、后 add $K，
//    0x081ce65e..0x081ce6a5）。4.4 前端会把纯指针形 `p + i*0xc + K` 折叠成
//    `p + (i*0xc + K)`（先 add 常量、后 add 基址，后两条再多 mov %ecx/lea）；
//    经 unsigned long 中转 `(unsigned long)(p + i*0xc) + K` 阻止该折叠，逐条复现 ORIG。
// ============================================================================
int Dispatcher_Client_Spec_Statistic::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CLIENT_SPEC_STATISTIC& m = static_cast<MSG_CLIENT_SPEC_STATISTIC&>(msg);
    packet.get_byte(m.m_specByte);
    packet.get_byte(m.m_spec.m_pad0[0]);
    packet.get_byte(m.m_spec.m_pad0[1]);
    for (int i = 0; i < (int)static_cast<unsigned char>(m.m_spec.m_pad0[1]); ++i) {
        packet.get_byte(m.m_spec.m_specs[i].m_category1);
        packet.get_int(m.m_spec.m_specs[i].m_category2);
        packet.get_int(m.m_spec.m_specs[i].m_category3);
    }
    return 0;
}