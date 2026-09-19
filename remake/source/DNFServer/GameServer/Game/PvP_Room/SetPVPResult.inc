int PvP_Room::SetPVPResult(int seat, const PvpResultType& result)
{
    // ORIG 0x085dc060：写 PvP_Room 内嵌数组 m_field78[seat] / m_field98[seat]
    // （lea 0x1c(seat),*4 后 mov 0x8(disp) → +0x78；+0x24 → +0x98）。
    // ORIG 085dc040/085dc043：js/jg 各落独立空块岛（nop;jmp end / nop;fall end），
    // 双 goto 保块（同 check_recv_pvp_rank 已验证模式）。
    // 残差仅 outgoing-args reserve 跨版本尺寸差（ORIG sub $0x4 vs 4.4 $0x14），
    // 指令序列全等，N 封顶。
    if (seat < 0)
        goto SET_RES_RANGE;
    if (seat > 7)
        goto SET_RES_RANGE2;
    m_field78[seat] = result.GetTotalScore();
    m_field98[seat] = result.m_avg[0];
SET_RES_RANGE2:
    ;
SET_RES_RANGE:
    ;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}