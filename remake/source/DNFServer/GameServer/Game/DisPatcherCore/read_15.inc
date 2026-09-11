// ORIG 0x081caf42 T _ZN23DisPatcher_WalkoutParty4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x202b, PRETTY, 0, 0)。
int DisPatcher_WalkoutParty::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_WALKOUT_PARTY& m = *reinterpret_cast<MSG_WALKOUT_PARTY*>(&msg);
    if (!packet.get_byte(m.m_walkoutSlot))                // +0x0d
    {
        return LineFunc(0x202b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
