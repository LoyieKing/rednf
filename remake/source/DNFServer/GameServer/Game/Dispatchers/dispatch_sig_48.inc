// [b28c-31] N82：ORIG 0x082251xx 复用 item 对象死亡后的栈槽（GetMoney 结果落
// -0x46），4.4 不复用槽且 money/-0x10 形态不同 ⇒ 槽复用策略跨版本（N82）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082250ec orig/ours=73/71 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN24Dispatcher_WithdrawMoney12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,-0x46(%ebp) | mov    -0x10(%ebp
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
        // [DispG1/48 负结论 2026-09-11] 试加具名 getMoney（ORIG 0x082251b5 有槽往返）
        //   ⇒ 实测 72/72 → **72/69**（劣化 3）⇒ 回滚。4.1 该槽复用 item 死亡区，4.4 不复用。
        user->m_historyLog.AccountCargoDelItem(
            item.m_addInfo, (int)cargo->GetMoney(), (int)money,
            (eItemDelReason)0x26);
    }
    return 0;
}