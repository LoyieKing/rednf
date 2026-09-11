// ORIG T 0x0845fdc0：4 字节名 + ushort + short + byte + finalize(true)。
void CDeathTower::CPacketHandler::appendPickupItem(char* name, unsigned short uid,
                                                   int v3, int v4)
{
    for (int i = 0; i < 4; ++i)
        put_byte((int)(signed char)name[i]);
    put_short((int)uid);
    put_short(v3);
    put_byte(v4);
    finalize(true);
}