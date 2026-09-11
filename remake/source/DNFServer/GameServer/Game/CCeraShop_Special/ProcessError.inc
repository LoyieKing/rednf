// ============================================================================
// ProcessError —— ORIG 0x08323978 (T)
// 语义：构造 0x43 错误包（header 1/0x43），按调用方错误码序列化，Send 后记
// cMyTrace "NEXON BILLING : Buy Error(%d,%u)"。
// C 反编译：put_header(1,0x43); put_byte(0); put_byte(0); put_short(param_2);
//           put_byte(param_7); put_int(param_3); put_int(param_4);
//           put_int(param_5); put_int(param_6); put_int(-1); finalize(true);
//           user->Send(packet); cMyTrace(...)。
// ============================================================================
void CCeraShop::ProcessError(CUser* user, unsigned short a, int b, unsigned int c,
                             int d, int e, char f)
{
    PacketGuard packet;
    packet.put_header(1, 0x43);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_short(a);
    packet.put_byte(f);
    packet.put_int(b);
    packet.put_int(c);
    packet.put_int(d);
    packet.put_int(e);
    packet.put_int(-1);
    packet.finalize(true);
    user->Send(packet);

    // b28d-D42 P227/P221：ORIG 0x08323978 中 trace 匿名槽(-0x18)浅于
    // PacketGuard(-0x24)，且 (int)a 在 ctor 前装载 %ebx 跨 ctor 保活
    // （`movzwl -0x2c(%ebp),%ebx` 先于 cMyTrace C1）⇒ 全表达式临时链形。
    cMyTrace(__PRETTY_FUNCTION__, 0xc2a, 0)(
        "NEXON BILLING : Buy Error(%d,%u)", (int)a, c);
}