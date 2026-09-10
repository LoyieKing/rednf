void Area::send_area_users(CUser* user)
{
    if (user->getCurCharacVill() != m_villageIndex ||
        user->get_area(false) != m_areaIndex)
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x18);
    packet.put_byte(m_villageIndex);
    packet.put_byte(user->get_area(true));

    int index = packet.get_index();
    int count;
    count = 0;    // P197③ 双段式（ORIG movl $0x0,-0xc 预清独立行）
    packet.put_short((int)m_UsersInArea.size());

    // [b28c-21] ORIG begin 调用之后才发 `movl $0x0,-0xc(%ebp)`（u 空预置）
    // ⇒ 迭代器初始化拆到循环外、u 声明置于其后（P0：u 有槽往返＝具名局部）。
    std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
    CUser* u = 0;
    for (; it != m_UsersInArea.end(); it++)
    {
        u = it->second;
        // [b28c-21] ORIG 086c2411 段：可见性谓词两次独立调用（P48：O0 无 CSE），
        // 第二臂为 `!vis2 && u == user` 短路形（xor$1;test;je 后 cmp）。
        if (u->IsCurCharacVisible() ||
            (!u->IsCurCharacVisible() && u == user))
        {
            packet.put_short(u->get_unique_id());
            packet.put_short(u->get_posX());
            packet.put_short(u->get_posY());
            packet.put_byte(u->get_direction());
            packet.put_byte(u->get_charac_visible_values());
            ++count;
        }
    }

    packet.put_short(index, count);
    packet.finalize(true);
    user->Send(packet);
