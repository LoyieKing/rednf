// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::OnTimerWalkOutLackUser（原 TU 第 2457 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

bool PvP_Room::OnTimerWalkOutLackUser()
{
    // ORIG 0x085dce28：Ghidra 双条件 `-1 < local_10 && -1 < local_10`——尾 cond
    // 用 not/shr$0x1f 物化（loop tail），body 头重复 `cmpl$0; js→nop 岛`
    // （P184 死块照抄）。双 guard 形（while(x>-1) + 体内 if(x<0) break）使
    // 4.4 复现全部 69 条含 not/shr 岛（合并 && 形被 bool 汇聚，P133）；
    // 残余 = OnWalkOut 虚 call callee-saved 互换 10 行（N7，同 get_user_list 判例），
    // NEAR 封顶。
    int local_10 = m_election.GetElected();
    while (local_10 > -1)
    {
        if (local_10 < 0)
            break;
        m_election.CancelCandidate(local_10);
        m_election.CancelVoter(local_10);
        ((IMatch*)m_matching)->OnWalkOut(this, local_10);
        if (!OnSetPVPSeatState(P_SEATS(local_10), local_10, ENUM_SEAT_STATE_0xfe,
                               (ENUM_WALKOUT_TYPE)3))
        {
            return 0;
        }
        local_10 = m_election.GetElected();
    }
    return 1;
}
