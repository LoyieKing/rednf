// ---------------------------------------------------------------------------
// SendLV70Buff(CUser*, unsigned long)
// ORIG T _Z12SendLV70BuffP5CUserm @0x0865defc sz0x1b2。
// 报告 docs/class_func_reports/_lt_global_gt_/SendLV70Buff.md（## 汇编+## 反编译 C）。
// 包 0x100（param→int, 0xe10）直发 + send_to_area（带 CBuffGroup）；
// 包 0x103（get_unique_id 低 16 位, param）send_to_area（无 belong）。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureE] AE 115/115（p167 CLEAR 25/25）：area/vill 去具名局部
//   （P0 表达式直传 %ebx/%esi），group/packet 槽 -0x10/-0x1c 全对齐。
void SendLV70Buff(CUser* user, unsigned long buff)
{
    CBuffGroup group(user);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x100);
    packet.put_byte(1);
    packet.put_int((int)buff);
    packet.put_int(0xe10);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet, &group);
    packet.clear();
    packet.put_header(0, 0x103);
    packet.put_short((int)(unsigned short)user->get_unique_id());
    packet.put_int((int)buff);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet);
}