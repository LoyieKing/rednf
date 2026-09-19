int BlueMarble::sendStartBlueMarble() const
{
    PacketGuard packet;
    packet.put_header(0, 0x1b0);
    packet.put_byte((int)(char)getBlueMarbleMap());
    packet.finalize(true);
    sendToBlueMarble(packet);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}