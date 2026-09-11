// ---------------------------------------------------------------------------
// ORIG 0x08268492：删共享消息 1/2/3 后广播三条空串公告（send_all_with_state 3）。
void CPowerWarEvent::CleanupPowerWarServerMessage()
{
    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        mgr->DeleteSpecificMessage(1);
        mgr->DeleteSpecificMessage(2);
        mgr->DeleteSpecificMessage(3);
        PacketGuard packet;
        packet.put_header(0, 0xc0);
        packet.put_byte(2);
        packet.put_byte(3);
        char* msg = "null string";
        int len = static_cast<int>(strlen(msg));
        for (int i = 0; i <= 2; ++i)
        {
            packet.put_byte(i + 1);
            packet.put_int(len);
            packet.put_str(msg, len);
            packet.put_int(len);
            packet.put_str(msg, len);
        }
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
}