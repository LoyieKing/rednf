// ORIG 0x082643b2（报告 Dispatcher_ScanBotByDll/read.md）[随行依赖]。
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x082643b2 orig/ours=51/51
// [P4-p167 符号性] ORIG callee 直证: get_short(unsigned short&)
//   （p167: MISSING _ZN9PacketBuf9get_shortERt / EXTRA …ERs）
int Dispatcher_ScanBotByDll::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x：裸偏移内联（无指针槽，帧 0x18）。
    if (!packet.get_int(*reinterpret_cast<int*>((char*)&msg + 0xd)))
    {
        return LineFunc(0x3d6, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>((char*)&msg + 0x11)))
    {
        return LineFunc(0x3db, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>((char*)&msg + 0x12)))
    {
        return LineFunc(0x3e0, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}