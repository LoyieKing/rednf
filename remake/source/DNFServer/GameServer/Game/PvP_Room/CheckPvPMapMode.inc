// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::CheckPvPMapMode（原 TU 第 2660 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] AE: P248 档①/P292 已落 —— IsDeathMatchMap bool 化后调用点
// `call; mov %al,-0x9` 直存与 ORIG 0x085dd936 全等（旧「test;setne 归一封顶」判词作废）。
bool PvP_Room::CheckPvPMapMode(CUser* user, int mapNo, bool& flag)
{
    if (m_pvpBattleMode == 6)
    {
        return 1;
    }
    // ORIG 085dd9xx：bool 型 cVar1（call 结果 al 直存 byte 槽、无归一）；
    // !cVar1 判定走 movzbl+xor $1 的 4.1 取反形。
    bool cVar1 = IsDeathMatchMap(mapNo, flag);   // bool 形：后两处 !cVar1 的 movzbl+xor 物化与 ORIG 同；
    // 差仅入口 mov%al 直存 vs 4.4 test;setne 归一（N bool ABI），净差 -2 优于 uchar 形 +4，
    // char 形实测 ndiff 12（!cVar1 折成 cmpb/jne，丢 movzbl+xor+je 四段×2）
    if ((cVar1 != 0) && (!flag) && (m_pvpBattleMode == 4))
    {
        return 1;
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 3))
    {
        m_pvpBattleMode = 4;
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 1))
    {
        set_pvp_mode(user, 4);
    }
    if ((cVar1 != 0) && (m_pvpBattleMode == 2))
    {
        m_pvpBattleMode = 5;
    }
    // ORIG 085dd9ef/085dda0x：!char-slot 形 = movzbl -0x9 + xor $1 + test（xor
    // 值化，非 cmp 归一）；(cVar1^1) 写法被 4.4 fold-const 折成 cmpb $1。
    if ((!cVar1) && (m_pvpBattleMode == 4))
    {
        set_pvp_mode(user, 1);
    }
    if ((!cVar1) && (m_pvpBattleMode == 5))
    {
        m_pvpBattleMode = 2;
    }
    return 1;
}
