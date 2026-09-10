// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvpResultType::GetTotalScore + PvP_Room::SetPVPResult（同组：P210② 帧继承，定义序不可分）（原 TU 第 4138 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N-帧量化] ORIG 0x085dc034 `sub $0x4` vs 4.4 `sub $0x14`：callee PvpResultType::GetTotalScore 定义在 PvpResultType.cpp（跨泳道，仅声明可见）⇒ 4.4 保守 outgoing 量化（P190 附注②）；指令序列全等、P167 CLEAR。
// [b28d-solo-PvP_Room] N-P190: ORIG 0x085dc034 sub $0x4 vs 4.4 $0x14 outgoing 量化；callee PvpResultType::GetTotalScore 跨 TU 仅声明
// [L10-2 P210②] GetTotalScore 定义迁入本 TU 并置于 caller 之前 ⇒ 4.4 精确继承
// outgoing（同 PvpResultType.cpp 内 PvpResultSet 三薄封装的前例）。原地址
// 0x085d5efe 与 PvP_Room 段相邻，属 ORIG 同 TU。
int PvpResultType::GetTotalScore() const
{
    return m_avg[11] + m_avg[12] + m_avg[13] + m_avg[14] + m_field6c +
           m_field70 + m_field74 + m_field78;
}

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
}
