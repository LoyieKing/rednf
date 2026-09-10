void Area::send_to_all(PacketGuard& packet, std::vector<CUser*>& users)
{
    // [b28d-solo-Area] while+continue 真循环形（ORIG 086c2f2c）：内层 users 循环
    // 死比较（P225 dead 变体）与匿名 sret 槽方向均已对齐。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    while (it != m_UsersInArea.end())
    {
        user = it->second;
        // ORIG 086c2f2c：内层对 users 逐一比较，结果未使用（死代码），保留遍历。
        for (std::vector<CUser*>::size_type i = 0; i < users.size(); ++i)
        {
            // [b28d-solo-Area] ORIG 086c2f95 `cmp; sete %al; test %al,%al` 后无分支
            // （死比较：4.1 照发 bool 物化、4.4 DCE）⇒ P225 dead 变体 while+break。
            while (users[i] == user)
                break;
        }
        if (user == 0 || user->get_state() <= 2)
        {
            it++;
            continue;
        }
        user->Send(packet);
        it++;
    }
