int Dispatcher_RedeemList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x822621c：state>2 时向用户发送兑换信息；否则返回 0
    if (user->get_state() <= 2)   // ORIG 0x822621c：无 user==0 检查
    {
        return 0;
    }
    user->send_RedeemInfo();
    return 0;
