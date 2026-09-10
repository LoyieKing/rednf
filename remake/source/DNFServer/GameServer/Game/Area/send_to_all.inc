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
