int PvP_Room::OnConnectP2PPvPTimeout()
{
    // 残余 2 行 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    if (!check_start_state())
    {
        ((IMatch*)m_matching)->OnConnectP2PTimeout(this);
    }
}