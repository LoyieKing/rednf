// ============================================================================
// nexon_packet::stHeader::stHeader（ORIG 0x086941cc W，magic=0xaa; length=0;
// cmd=0）。定义原在 nexon_packet.cpp，迁入本 TU（stPacketC1 的 callee）——
// stPacketC1 所在 TU 不再含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing
// 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
// ============================================================================
nexon_packet::stHeader::stHeader()
    : m_magic(0xaa), m_length(0), m_cmd(0)
{
}