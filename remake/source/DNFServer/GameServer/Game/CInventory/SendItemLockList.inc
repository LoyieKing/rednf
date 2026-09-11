void CInventory::SendItemLockList(const Inven_Item* items, int count,
                                  ENUM_ITEMSPACE space) const
{
    // b28d-solo-CInventory：113/115，槽平移 4/8；P212 全头部声明实测无改善已回退 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：帧/槽残差（112/114）。
    if (m_pParent == 0)
        return;
    PacketGuard packet;
    // ORIG 不落 buf 局部：每调用点重新 lea packet（P157 逐字段重读）。
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xfb);
    int countIdx = ((InterfacePacketBuf*)&packet)->get_index();
    int cnt = 0;
    ((InterfacePacketBuf*)&packet)->put_short(cnt);   // ORIG 以 cnt 变量传参（此刻为 0）
    for (int i = 0; i < count; ++i) {
        if (items[i].m_amp.GetLock() != 0) {
            ((InterfacePacketBuf*)&packet)->put_byte((int)space);
            ((InterfacePacketBuf*)&packet)->put_short(i);
            // [P0/P183 删具名局部] ORIG 无 expand 槽：call GetCharacExpandDataR 的结果
            // 直接作 receiver（`mov %ebx,0x8(%esp)` 传锁值，无 -0xNN 往返）⇒ 单条链式表达式。
            reinterpret_cast<const item_lock::CItemLock*>(
                m_pParent->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2))
                ->MakeItemLockPacket(packet, (unsigned char)items[i].m_amp.GetLock());
            ++cnt;
        }
    }
    ((InterfacePacketBuf*)&packet)->put_byte(countIdx, cnt);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    m_pParent->Send(packet);
}