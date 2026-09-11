int PvP_Room::end_pvp()
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    ((IMatch*)m_matching)->OnEndFight(this);
}