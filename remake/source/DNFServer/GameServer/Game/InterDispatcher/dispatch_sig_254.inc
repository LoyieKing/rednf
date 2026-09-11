// [b28d-lane2] verdict=DIFF ndiff=12（ORIG 0x084e3232）：块序重排(N104/P223)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：`ret = 0;` 移入 PacketGuard
// 作用域内，复现 ORIG「ret 物化 → dtor → jmp 汇合」顺序）。
int Inter_SendPeriodicMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG 0x084e3232：recv 槽 -0xc 处处 reload（(unsigned long) 往返破 4.4 SSA 传播，技巧26）；
    // 返回经 %ebx 物化（EH 函数 4.1 尾置 mov$0,%ebx + end mov%ebx,%eax）。
    // 正常路径 dtor：ORIG 4.1 inline epilogue（send 后 mov$0,%ebx + dtor + jmp），
    // 4.4 恒合并尾岛共享 dtor call（N 类：4.1 EH 双份 dtor epilogue vs 4.4 merge）。
    // [b28c-32] P0：register asm("ebx") 绑定清除，改 P231 纯 register（合法，无 asm）。
    register int ret;
    char* recv = reinterpret_cast<char*>((unsigned long)data);   // ORIG +0x0a 物化 -0xc
    if (*(recv + 0xa) == '\0')   // ORIG +0x10 add $0xa; movzbl 两步装载；false-branch 先写（技巧17：ORIG fall=island，body 为 jne taken）
    {
        ret = 0;
    }
    else
    {
        {
            PacketGuard packet;
            packet.put_header(0, 299);
            packet.put_int(strlen(recv + 0xa));
            packet.put_str(recv + 0xa, strlen(recv + 0xa));
            packet.finalize(true);
            G_GameWorld()->send_all_with_state(packet, 3);
            ret = 0;
        }
    }
    return ret;
}