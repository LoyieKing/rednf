void GameWorld::move_position(CUser* user, int x, int y, int z,
                              unsigned short dir)
{
    // [b28d-solo-GameWorld] _ZN9GameWorld13move_positionEP5CUseriiit verdict=AE 152=152
    // 修法：P221 全表达式临时链 cMyTrace(...)(...)（原具名 trace 与 PacketGuard
    // 槽互换 N93/N82；临时链复现 ORIG trace=-0x28 浅于 guard=-0x34）。
    // [b28d-solo-GameWorld] _ZN9GameWorld13move_positionEP5CUseriiit：ORIG 0x086c5712
    // `call IsCurCharacGhost; test %al,%al; jne 跳过` ⇒ 早退形（bool 直判，无 xor）。
    if (user == NULL)
    {
        return;
    }
    if (user->IsCurCharacGhost())
    {
        return;
    }
    user->set_position((unsigned short)x, (unsigned short)y, (char)z);

    PacketGuard packet;
    packet.put_header(0, 0x16);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(x);
    packet.put_short(y);
    packet.put_byte(z);
    packet.put_short(dir);
    packet.finalize(true);

    int vill = (int)user->getCurCharacVill();
    int area = user->get_area(false);
    if (check_valid_area(vill, area))
    {
        Village* village = getUserVillage(user);
        // ORIG 0x086c57e2 `cmpl $0,-0x10; je 后置 trace 块` ⇒ 正极性 if(village!=0) 直落 body。
        if (village != NULL)
        {
            Area* pArea = village->getArea(area);
            if (pArea != NULL)
            {
                // ORIG 0x086c5814 `test %al,%al; je Send 块` ⇒ 正极性 if(visible) 直落 send_to_all。
                if (user->IsCurCharacVisible())
                {
                    pArea->send_to_all(packet);
                }
                else
                {
                    user->Send(packet);
                }
            }
        }
        else
        {
            // [b28d-solo-GameWorld] P221 全表达式临时链翻 sibling 块槽方向
            // （ORIG trace 槽 -0x28 浅于 PacketGuard -0x34）。
            cMyTrace(__PRETTY_FUNCTION__, 0x5ec, 5)("%d %d", vill, area);
        }
    }
