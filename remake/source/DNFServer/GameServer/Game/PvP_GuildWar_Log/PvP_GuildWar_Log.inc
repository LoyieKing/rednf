PvP_GuildWar_Log::PvP_GuildWar_Log()
    : m_startTime(0)
{
    // b28d-M2S3（推翻旧 N 判词）：ORIG 0x085dda4b 为 `sub $0x10`。
    // 帧-死局部穷举（4.4.7 / 446 / 444 三后端读数一致）：空体 ⇒ `sub $0x4`；
    // char[4] / [8] / [0xc] ⇒ 恒 `sub $0x14`。⇒ 0x10 不可达（4.4 帧只有
    // 0x4/0x14 两档）⇒ 保留最小死局部取最近帧（差 4B，N82 帧量化族）。
    // [N1B 2026-09-11 复核·帧量化规则] /tmp/frametest{,2}.cpp 扫 14 档（1/2/3/4/5/
    // 6/7/8/9/10/12/16/20 字节 + double + long double，同 TU 旗标）：空体 ⇒ 0x4；
    // 1..16 字节 ⇒ 0x14；20 字节 ⇒ 0x24 = 4 + round_up(locals,16) ⇒ 帧值集合为
    // {4} ∪ {4+16k}，**0x10 结构性不可达**（需 round_up=0xc）。ORIG 4.1 的 0x10 属
    // 跨版本帧量化差（N82），本件唯一残差（25/25 指令逐条同形、仅 sub/add 立即数
    // 0x10 vs 0x14）⇒ NEAR 封顶。
    char framePad[4];
    (void)framePad;
}