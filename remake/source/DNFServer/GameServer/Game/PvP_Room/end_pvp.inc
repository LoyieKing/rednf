int PvP_Room::end_pvp()
{
    // N7 封顶（虚调用 callee 寄存器互换，见 get_user_list 注释）。
    ((IMatch*)m_matching)->OnEndFight(this);
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}