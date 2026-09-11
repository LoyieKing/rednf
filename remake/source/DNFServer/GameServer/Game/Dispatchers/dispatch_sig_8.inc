// ORIG 0x08226250（报告 Dispatcher_Redeem/dispatch_sig.md）：
// state>2 受理；get_byte 读 1 字节（有符号）作 redeemId 调 Redeem_Item。
int Dispatcher_Redeem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG：state<=2 → mov $0,%eax;jmp 尾（早退），体内 char 槽无初始化（无 movb $0 物化）
    if (user->get_state() <= 2)
    {
        return 0;
    }
    {
        char redeemId;
        packet.get_byte(redeemId);
        user->Redeem_Item((int)redeemId);
    }
    return 0;
}