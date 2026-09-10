// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::VoteLackUser（原 TU 第 2415 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N7] ORIG 0x085dcf1a：key→%ebx / m_index→%esi（0x085dd01a/085dd014），4.4 反配；asm 绑定与裸 register（P231）两形实测均不翻（P248 条件②不成立）⇒ 寄存器角色互换封顶，语义全等、P167 CLEAR。
// [b28d-solo-PvP_Room] N7-N109: ORIG 0x085dcf1a key->ebx/m_index->esi，4.4 反配；(int)/unsigned 两旋钮实测不翻
bool PvP_Room::VoteLackUser(CUser* user, int vote)   // [b28c-16 SIGFIX] ORIG 串 bool
{
    if ((vote < 0) || (7 < vote))
    {
        LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                              0x116d, "VoteSlotNo(%d) < 0 || MAX_PVP_PLAYER < VoteSlotNo", vote);
        return false;
    }
    CUser* u = P_SEATS(vote);
    // ORIG 085dccbe: mov 0xc(%ebp),%eax（user 先载 reg）/ cmp -0x10(%ebp),%eax
    // —— 操作数序 user==u；u==user 反产 slot 先载。
    if ((user == u) || (u == 0))
    {
        return false;
    }
    int iVar2 = get_user_seat(user);
    if ((iVar2 < 0) || (8 < iVar2))
    {
        LogManager::logFormat(1, "pvp.cpp", __PRETTY_FUNCTION__,
                              0x117b, "UserSlotNo(%d) < 0 || MAX_PVP_PLAYER < UserSlotNo", iVar2);
        return false;
    }
    m_election.Vote(iVar2, vote);
    // P24/P52：ORIG 链式内联实参（0x085dd01a），key 先物化 ebx、m_index 缓存 esi，
    // G_TimerQueue() 置后，跨调用 callee-saved 存活
    // ORIG 085dd00e/085dd014：key→%ebx、index→%esi（4.4 反产 = N7 互换，
    // asm("ebx")/裸 register 实测均不翻，P248 条件②不成立）⇒ 保持直写。
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1, m_index,
                                            (TIMER_MESSAGE)0x31, 0x9c4LL,
                                            (int)gen_timer_key((TIMER_MESSAGE)0x31), 0);
    return true;
}
