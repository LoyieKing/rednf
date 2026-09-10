void CPartyTelePort::send_teleport_effect_at_equal_place(int param)
{
    if (m_party == 0)
    {
        return;
    }
    CUser* manager = (CUser*)m_party->getManager();
    if (manager == 0)
    {
        return;
    }
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        return;
    }

    PacketGuard pg;

    pg.put_header(0, 0x17b);
    pg.put_byte((int)(char)param);

    std::vector<unsigned short> users;
    for (int i = 0; i <= 3; ++i)
    {
        // [b28d-solo-CPartyTelePort] ORIG 0x85bc6a7：`call; xor $1; test;
        // jne <nop 岛>` + body 尾 jmp + nop = 4.1 同层多 continue 岛
        // （P223）：源码为两条独立 continue 守卫。
        if (!m_party->checkValidUser(i))
        {
            continue;
        }
        CUser* u = (CUser*)m_party->get_user(i);
        if (u != 0)
        {
            users.push_back(u->get_unique_id());
        }
    }

    char cnt = (char)users.size();
    pg.put_byte((int)cnt);
    for (int j = 0; j < (int)cnt; ++j)
    {
        pg.put_short((int)users[j]);
    }
    pg.finalize(true);

    if (manager->get_state() == 3)
    {
        // [b28d-solo-CPartyTelePort] ORIG 0x85bc7e3：get_area 结果留 %ebx、
        // getCurCharacVill 留 %esi（P0 无槽往返 / P52 链式 receiver 单语句）。
        G_GameWorld()->send_to_area((int)manager->getCurCharacVill(),
                                    manager->get_area(false), pg);
    }
