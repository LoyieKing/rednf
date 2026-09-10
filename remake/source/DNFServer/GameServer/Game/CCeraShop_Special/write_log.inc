// ---- write_log —— ORIG 0x0827d2df ----
// type==0x53/0x57：重算+发表情；type==8：组 2 包并广播 basic_info。
// b28c-L20: AE — P17/P46：ORIG 0x827d2e7 为 switch 线性链（`mov 0xc(%ebp),%eax`
// 一次载入 + 三 `cmp $imm,%eax/je` 复用 + 两 je 同目标 + 尾 jmp）；if/else-if
// 在 4.4 生 `cmpl $imm,mem` 直比 + 第二条件 jne 落体（旧 N18a/b 判词作废）。
void premium_helper_function::write_log(CUser* user, int type)
{
    switch (type)
    {
    case 0x53:
    case 0x57:
        user->ReCalcChattingEmoticon();
        user->SendChattingEmoticon();
        break;
    case 8:
    {
        PacketGuard packet;
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, '\0');
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);
        break;
    }
    }
}

