int Dispatcher_WithdrawMoney::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    unsigned int money = 0;
    if (!packet.get_int(money))
    {
        return LineFunc(0xdfda,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!user->IsExistAccountCargo())
    {
        return 0;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    if (cargo->WithdrawMoney(money))
    {
        Inven_Item item;
        item.m_addInfo = 0;
        // ORIG 0x0820xxxx：GetMoney 结果两段中转 `mov %eax,-0x46(%ebp)`…
        // `mov -0x46(%ebp),%edx`（4.1 复用 item 对象死亡后的栈槽，4.4 不复用 ⇒
        // 该 +0x79700 this 占 %ebx / money 占 -0x10 的形态在单 TU 内不可复现）。
        user->m_historyLog.AccountCargoDelItem(
            item.m_addInfo, (int)cargo->GetMoney(), (int)money,
            (eItemDelReason)0x26);
    }
    return 0;
