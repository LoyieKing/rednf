void CCargo::SendItemLockList() const
{
    if (m_pParent == 0)
        return;
    PacketGuard packet;
    // [b28d-solo-CCargo] P183 地址重物化：ORIG 每处调用都重发 `lea -0x1c(%ebp),%eax`
    // （无 buf 指针局部）；P244：put_short 实参是具名 count（ORIG `mov -0x10(%ebp),%eax;
    // mov %eax,0x4(%esp)`，非立即数 0）。
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xfb);
    int countIdx = ((InterfacePacketBuf*)&packet)->get_index();
    int count = 0;
    ((InterfacePacketBuf*)&packet)->put_short(count);
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_amp.GetLock() != 0) {
            ((InterfacePacketBuf*)&packet)->put_byte(2);
            ((InterfacePacketBuf*)&packet)->put_short(i);
            // [b28d-solo-CCargo] ORIG `call GetLock; movzbl %al,%ebx; …call
            // GetCharacExpandDataR; mov %ebx,0x8(%esp)`（实参先于 receiver 求值、
            // 零扩展升格 %ebx）⇒ P200 内联实参形（禁具名/register 中间局部）。
            reinterpret_cast<item_lock::CItemLock*>(
                m_pParent->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2))
                ->MakeItemLockPacket(packet, m_pItems[i].m_amp.GetLock());
            ++count;
        }
    }
    ((InterfacePacketBuf*)&packet)->put_byte(countIdx, count);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    m_pParent->Send(packet);
