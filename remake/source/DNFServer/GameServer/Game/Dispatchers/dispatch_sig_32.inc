int Dispatcher_DepositMoney::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    unsigned int money = 0;
    if (!packet.get_int(money))
    {
        return LineFunc(0xdfc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!user->IsExistAccountCargo())
    {
        return 0;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    if (cargo->DepositMoney(money))                        // ORIG test %al; je（CUser.h 已 bool 化）
    {
        // 【batch-17 S池 P183/两步物化】ORIG 0x08225090..0x082250d4：日志三值
        // 不是字面直传，而是**先默认构造一个 Inven_Item 临时**（`lea -0x4d;
        // call Inven_Item::C2`），把 itemIdx 写进对象 +0x02（`movl $0,-0x4b`）、
        // 把 GetMoney() 写进 +0x07（`call GetMoney; mov %eax,-0x46`），随后按
        // 右→左实参序把 money/GetMoney()/itemIdx 读回落参槽。用字面量直传会少
        // 掉整个临时对象（差集 ORIG Inven_Item C2 ×1、实测 -5 insn）。
        Inven_Item logItem;
        logItem.m_addInfo = 0;
        logItem.m_addInfo2 = cargo->GetMoney();
        user->m_historyLog.AccountCargoAddItem(
            (int)logItem.m_addInfo, (int)logItem.m_addInfo2,
            (int)money, (eItemAddReason)0x2d);
    }
    return 0;
}