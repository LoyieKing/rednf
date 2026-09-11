// ----------------------------------------------------------------------------
// ORIG T 0x0812fdd4（gotoStage.md 全文；静态语义，asm 不使用 this）：
//   getUserVillage 判空 → cMyTrace(0x585 行, vill) 返回；getArea(get_area(false))
//   判空 → cMyTrace(0x58c 行, area) 返回；否则 erase_user →
//   包1 0x17{short uid, byte vill, byte 0xff, short posX, short posY,
//   byte direction, byte visible_values} Send → clear →
//   包2 3{byte 1, short uid, byte 1} Send + send_all(guard, user) → set_state(0xd)。
// ----------------------------------------------------------------------------
// 头声明 static（ORIG 无 this）；类外定义不带 static。
// [b28d-D9] 槽差/P227：trace 临时链位置 + 槽序差（ORIG 0x0812fdd4 sz 0x000002fa）
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
}