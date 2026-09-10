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
