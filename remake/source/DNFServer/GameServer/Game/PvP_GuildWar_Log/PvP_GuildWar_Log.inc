PvP_GuildWar_Log::PvP_GuildWar_Log()
    : m_startTime(0)
{
    // [b28d-23 N 封顶] ORIG 0x085dda4b `sub $0x10`（0xc 局部 + 4 outgoing）。
    // P232 补 char[0xc] 后 4.4 发 `sub $0x14`（实测）：4.4 带 call 帧恒 16k+4
    // 量化（P190 附注 b28 BL11：0x4/0x14/0x24…），ORIG 的 0x10 非量化值不可达；
    // 保留 0xc 死局部取最近帧（差 4），语义等价（framePad 不读写）。
    char framePad[0xc];
    (void)framePad;
