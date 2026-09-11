// ORIG 0x081c5d78 T _ZN21DisPatcher_PvPTimeout4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_PvPTimeout/read.md 逐块映射）：循环 i=0..7 共 8 次
// get_int → msg+0xd+i*4；失败 LineFunc(0x14a6, PRETTY, 0, 0)。
int DisPatcher_PvPTimeout::read(PacketBuf& packet, MSG_BASE& msg)

{
    // MSG_PVP_TIMEOUT（PacketTypes.h:1087 冻结）+0xd 起为 8 个 int 席位态。
    // ORIG 入口 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`；循环体
    // `mov -0xc,%eax; shl $0x2; add -0x10(%ebp),%eax; add $0xd,%eax` = 数组成员
    // 下标形 `req->seat[i]`（probe 实证：裸指针加法恒被 fold-const 折成 +0xd 先行）。
    #pragma pack(push, 1)
    struct ViewPacked { char head[13]; int seat[8]; };
    #pragma pack(pop)
    ViewPacked* req = reinterpret_cast<ViewPacked*>(&msg);
    for (int i = 0; i <= 7; ++i)
    {
        if (!packet.get_int(req->seat[i]))
        {
            return LineFunc(0x14a6,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}
