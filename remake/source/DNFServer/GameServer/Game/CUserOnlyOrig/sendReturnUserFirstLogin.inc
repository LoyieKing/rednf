// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0868a9a4：调用点/形态已对齐（本波修法见函数内注释）。
void CUser::sendReturnUserFirstLogin()
{
    // sendReturnUserFirstLogin.md @0x0868a9a4：IsReturnUser && IsReturnUserFirstLogin
    // 双门槛后发 noti 0x1bc 空载荷包。
    // ORIG 0x0868a9b7：`test;je 9d1 / test;je 9d1 / mov $1;jmp;9d1:mov $0; 9d6:test %al,%al;je`
    // ⇒ && 短路物化到 %al（无栈槽）。
    // [b28d-solo-CUserOnlyOrig P226/P0] 旧具名 ok 生 -0x9 槽 + cmpb 回读、
    // packet 槽整体下移 4（-0x14→-0x18）⇒ 语句表达式包住整个条件。
    if (({ IsReturnUser() && IsReturnUserFirstLogin(); })) {
    PacketGuard packet;
    packet.put_header(0, 0x1bc);
    packet.finalize(true);
    Send(packet);
    }
}