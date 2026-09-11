// ============================================================================
// 1. Dispatcher_Avatar_Disjoint_Random_Reward::read  ORIG 0x081de9da
//    LineFunc(0x5041, pretty, 0, 0)
// ============================================================================
int Dispatcher_Avatar_Disjoint_Random_Reward::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet;
    (void)msg;
    double framePad; (void)&framePad;   // P190: ORIG 帧 0x28 = 4.4 outgoing 0x18 + double 8 对齐撑帧
    return LineFunc(0x5041,
                    __PRETTY_FUNCTION__,
                    0, 0);
}