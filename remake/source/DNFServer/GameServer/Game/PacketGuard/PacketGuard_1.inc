// ORIG T 0x0858de4a（size 0x36，17 insns）：本 ctor 在 ORIG 里**不带** EH landing
// pad（同 TU 的 PacketGuard() @0x0858dd4c 却带）——依据基类 ~InterfacePacketBuf
// 非平凡，唯一自洽解释是 InterfacePacketBuf::AcquirePacketBuf 声明为不抛
// （已在 InterfacePacketBuf.h 落 throw()，实测本函数 32→17 insns，逐条同构 AE）。
PacketGuard::PacketGuard(PacketBuf* packetBuf)
    : InterfacePacketBuf(), m_pool(0), m_external(true)
{
    AcquirePacketBuf(packetBuf);
}