// ORIG 0x08264574（报告 Dispatcher_MotionHack/read.md）[随行依赖]：
// get_byte(msg+0xd) / get_int(msg+0xe) / get_int(msg+0x12)。
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x08264574 orig/ours=51/51
// [P4-p167 符号性] ORIG callee 直证: get_byte(unsigned char&) / get_int(unsigned int&) x2
//   （p167 差集: MISSING _ZN9PacketBuf7get_intERj x2 + _ZN9PacketBuf8get_byteERh x1）
int Dispatcher_MotionHack::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>((char*)&msg + 0xd)))
    {
        return LineFunc(0x414, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>((char*)&msg + 0xe)))
    {
        return LineFunc(0x416, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>((char*)&msg + 0x12)))
    {
        return LineFunc(0x418, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}