void StageControl::gotoStage(CUser* user)
{
    Village* vill = G_GameWorld()->getUserVillage(user);
    if (vill == 0)
    {
        // [b28d-solo-StageControl] P227：ORIG 在 ctor 前求 vararg 实参并以 %ebx 跨
        //   ctor 保活（`call getCurCharacVill; movsbl %al,%ebx; ... call C1; mov %ebx,
        //   0x8(%esp)`）⇒ 全表达式临时链形。
        cMyTrace(__PRETTY_FUNCTION__, 0x585, 5)(
            "%d", (int)user->getCurCharacVill());
        return;
    }
    // [b23S3C] P48：ORIG 两次独立 get_area(false)——+0x80 结果 eax 直落
    // getArea 实参槽（无 areaId 槽）、判空分支 +0xab 再 call 一次喂 trace。
    Area* area = vill->getArea(user->get_area(false));
    if (area == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x58c, 5)(
            "%d", user->get_area(false));
        return;
    }

    area->erase_user(user);
    PacketGuard guard;
    guard.put_header(0, 0x17);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_byte(user->getCurCharacVill());
    guard.put_byte(0xff);
    guard.put_short(user->get_posX() & 0xffff);
    guard.put_short(user->get_posY() & 0xffff);
    guard.put_byte(user->get_direction());
    guard.put_byte(user->get_charac_visible_values());
    guard.finalize(true);
    user->Send(guard);

    guard.clear();
    guard.put_header(0, 3);
    guard.put_byte(1);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    G_GameWorld()->send_all(guard, user);
    user->set_state((ch_state)0xd);
