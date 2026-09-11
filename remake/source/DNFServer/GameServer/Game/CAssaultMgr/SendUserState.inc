// ORIG T 0x82e83c4：SendUserState(CUser*) —— 组 0x03 状态包并全服广播
// （报告 pvp_assault__CAssaultPlace/SendUserState.md；ORIG 调用点仅压 CUser*，
//   头文件声明为静态形态）。
void CAssaultPlace::SendUserState(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_byte(user->GetUserState());
    packet.finalize(true);
    // ORIG 0x82e845c：G_GameWorld 结果 %eax 直通 this 实参（无 world 槽，
    // 其余实参用 %edx 装载）⇒ P52/P134 链式内联（N96 绑定已清）。
    G_GameWorld()->send_all(packet, user);
}