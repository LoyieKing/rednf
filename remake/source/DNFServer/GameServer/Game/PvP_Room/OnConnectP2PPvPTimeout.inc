int PvP_Room::OnConnectP2PPvPTimeout()
{
    // 残余 2 行 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    if (!check_start_state())
    {
        ((IMatch*)m_matching)->OnConnectP2PTimeout(this);
    }
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}