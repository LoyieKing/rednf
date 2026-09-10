// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::OnConnectP2PPvPTimeout（原 TU 第 2448 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::OnConnectP2PPvPTimeout()
{
    // 残余 2 行 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    if (!check_start_state())
    {
        ((IMatch*)m_matching)->OnConnectP2PTimeout(this);
    }
}
