int PvP_Room::get_extra_win_point(int team)
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    return ((IMatch*)m_matching)->GetExtraWinPoint(team);
}