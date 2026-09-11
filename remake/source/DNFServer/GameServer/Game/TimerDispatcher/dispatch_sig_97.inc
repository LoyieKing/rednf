// ORIG TimerVendingMachineItemNotify::dispatch_sig @ 0x8637f40 (0x134)
bool TimerVendingMachineItemNotify::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08637f40：结果驻留 %ebx；true 块 `result=1` 先于 dtor，
    // false 块 mov $0 紧跟 cmp（无提前初始化）。
    // 4.4 布局：`if (user == 0)` 写守卫（jne 跳过 inline mov$0 岛）复现 ORIG；
    // `if (user != 0)` 则 je 跳后置岛。
    // [P7HeaderAxis 2026-09-11] bool 化（ORIG 整族 dispatch_sig 返回 bool）：ORIG 尾 `mov %ebx,%eax` 直返、无 test/setne。
    register bool result;
    if (user == 0)
    {
        result = 0;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(0, 0x56);
        packet.put_byte(3);
        packet.put_byte(1);
        packet.put_short(user->get_unique_id() & 0xffff);   // movzwl 零扩展
        packet.put_item_idx(p2);
        packet.put_byte(0);
        packet.put_int(p3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        result = 1;
    }
    return result;
}