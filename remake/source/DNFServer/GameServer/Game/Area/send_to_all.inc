// ---------------------------------------------------------------------------
// send_to_all：向 state > 2 的在线用户广播包
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 9=9；70=70）。修法：P50+P28 后置 it++ +
// **while+continue 真循环形**——一条形翻正全部 5 个同族循环的匿名 sret 槽方向（旧判
// N93/N98 作废）。
void Area::send_to_all(PacketGuard& packet)
{
    // [b28d-solo-Area] while+continue 真循环形（ORIG 086c2d2a）：user 预清 +
    // P50 先声明后赋值 + P28 后置 it++；原 goto 形的 N93 sret 槽号残差已消。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    while (it != m_UsersInArea.end())
    {
        user = it->second;
        if (user == 0 || user->get_state() <= 2)
        {
            it++;
            continue;
        }
        user->Send(packet);
        it++;
    }
}