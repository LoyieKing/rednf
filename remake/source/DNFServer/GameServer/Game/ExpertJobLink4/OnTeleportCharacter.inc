// ---- CAlchemist::OnTeleportCharacter（报告 OnTeleportCharacter.md，
//      ORIG T 0x085d1424 sz 0x26f）----
// [L2Free4to15.S3 复核] 唯一残差 = 符号位测试 lowering：ORIG 0x085d1453
// `mov 0x10(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je` vs 4.4
// `cmpl $0x0,0x10(%ebp); jns`（ndiff 6、gap 2）。本轮再试 `(unsigned)area>>31`
// 一形仍被 4.4 前端折成 cmpl/jns（连同旧注释四形共 5 形）⇒ N97 族封顶。
// GI 假缺口（ORIG 该符号 493 实例，工具按名错配取首个；我方 16 条纯 ioinit 正确）。
bool CAlchemist::OnTeleportCharacter(CUser* user, int vill, int area, int x,
                                     int y, bool useGate)
{
    if (useGate)
    {
        area = G_GameWorld()->get_gate_area(vill);
        // [b28d-solo-ExpertJobLink4 N86b/N97]：ORIG 0x085d1453 `mov area,%eax;
        // shr $0x1f,%eax; test %al,%al; je`（4.1 符号位 lowering）；4.4 对
        // 符号位测试四形（`area<0` / `(unsigned)area>>31` / `area&0x80000000`
        // / `(unsigned)area/0x80000000u`）恒折 `cmpl $0x0; jns`（-2 条）⇒ 封顶。
        if (area < 0)
            return 0;

        if (!G_GameWorld()->get_area_center_point(vill, area, x, y))
        {
            // P139：ORIG 0x085d14a7 传 _ZZ…E19__PRETTY_FUNCTION__ 局部静态。
            // P221：全表达式临时链翻 sibling 块槽方向（ORIG trace 槽 -0x18 浅于
            // PacketGuard -0x24；具名 trace 在 4.4 下占 -0x24）。
            cMyTrace(__PRETTY_FUNCTION__, 0x8b, 5)(
                "get_area_center_point() call failed");
            return 0;
        }
    }

    // ORIG 0x085d14d7：直接 if 判断（无 needMove 中间变量）。
    if ((char)user->getCurCharacVill() != vill ||
        user->get_area(false) != area)
    {
        if (!G_GameWorld()->move_area(user, vill, area, x, y, 0, false, 0, 0, 0))
            return 0;
    }
    else
    {
        user->set_position((unsigned short)x, (unsigned short)y, '\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short(x);
    packet.put_short(y);
    packet.put_byte(0);
    packet.finalize(true);

    if (vill != 0 || area != 0)
        G_GameWorld()->send_to_area(vill, area, packet);

    return 1;
}