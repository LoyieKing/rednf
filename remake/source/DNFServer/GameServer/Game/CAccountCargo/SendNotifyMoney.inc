// ---- 发包 ----
void CAccountCargo::SendNotifyMoney(ENUM_CMDPACKET cmd)
{
    PacketGuard packet;
    // ORIG：无 buf 槽，每调用点重新 lea packet 地址（P0 右值直传）。
    ((InterfacePacketBuf*)&packet)->put_header(1, cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_int(m_money);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    m_pUser->Send(packet);
}