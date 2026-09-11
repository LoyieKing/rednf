// ---------------------------------------------------------------------------
// BuyFail —— ORIG T 0x0819e50e（报告 BuyFail.md 全文转写）。
// ---------------------------------------------------------------------------
void Arad_MileageProcess::BuyFail(CUser* user, short errCode, int a3, int a4, int a5)
{
    PacketGuard packet;
    // b28c-28：两值必须是** PacketGuard ctor 之后**初始化的非 const 局部——
    // ORIG 0x0819e50e+0x24 在 put_header 前才发 `movl $0xb,-0x10(%ebp)` /
    // `movb $0x0,-0x9(%ebp)`，且两处实参从槽装载
    // （`movsbl -0x9,%eax; mov %eax,0x4(%esp)`、`mov -0x10,%eax; mov %eax,0x4(%esp)`）；
    // const 形被前端折叠成 `movl $imm,0x4(%esp)`（少 2 条）。
    int resultCode = 0xb;        // ORIG -0x10（int 槽）
    char giftFlag = 0;           // ORIG -0x9（signed char 槽 ⇒ movsbl）
    packet.put_header(1, 0x43);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_short((int)errCode);
    packet.put_byte((int)giftFlag);
    packet.put_int(resultCode);
    packet.put_int(a3);
    packet.put_int(a4);
    packet.put_int(a5);
    packet.put_int(-1);
    packet.finalize(true);
    user->Send(packet);
}