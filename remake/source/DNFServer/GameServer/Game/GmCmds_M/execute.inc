void CMoveCharacCmd::execute()
{
    // P48：ORIG 每用点 fresh G_GameWorld()（find/move_area/send_to_area 各调）
    CUser* target =
        G_GameWorld()->find_user_by_charac_name(m_str0.c_str());
    if (target == 0)
    {
        return;
    }
    if (target->getCurCharacR() == 0)                       // ORIG CUserCharacInfo::getCurCharacR 判 0
    {
        return;
    }
    int vill = static_cast<int>(target->getCurCharacVill());
    int area = target->get_area(false);
    unsigned int x = target->get_posX() & 0xffff;
    unsigned int y = target->get_posY() & 0xffff;

    // ORIG(0x084a8dd4)：同 vill && 同 area → set_position，否则 move_area。
    // [G5 2026-09-10 P167/P183 真缺口修复] 旧码 `CUser* self = GetUser();` 把 5 个用点
    //   合并成 1 次调用；p167 实证 ORIG 调 `CCommand::GetUser()` **5 次**
    //   （MISSING 4×，OUR calls 27 vs ORIG 31）⇒ 删具名缓存，逐用点 fresh 重发
    //   （P48/P183：ORIG 在实参位直读、无 self 槽）。
    if (static_cast<int>(GetUser()->getCurCharacVill()) == vill &&
        GetUser()->get_area(false) == area)
    {
        GetUser()->set_position(static_cast<unsigned short>(x),
                                static_cast<unsigned short>(y), 0);
    }
    else
    {
        G_GameWorld()->move_area(GetUser(), vill, area,
                                 static_cast<int>(x), static_cast<int>(y),
                                 0, false, 0, 0, 0);
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(static_cast<int>(GetUser()->get_unique_id() & 0xffff));
    packet.put_short(static_cast<int>(x));
    packet.put_short(static_cast<int>(y));
    packet.put_byte(0);
    packet.finalize(true);
    if (vill != 0 || area != 0)
    {
        G_GameWorld()->send_to_area(vill, area, packet);
    }
