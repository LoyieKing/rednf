void CDeathTower::_pickupItemMoney(CUser* user, int memberCnt, const map_item& item, int* money1, int* money2)
{
    // ORIG 0x8466b02 尾 0x8466cf8..cf0 无 mov %eax 置值 —— 返回语义 void（mangled 无返回类型）。
    int money = item.m_item.get_add_info();
    int base = money / memberCnt;
    int remainder = money % memberCnt;
    int total = base + remainder;
    // ORIG 0x8466b42/8466b45：base 出循环前再拷贝一份 -0x1c（独立槽），
    // 循环内三元的 else 分支读该拷贝（0x8466bb4）而非 base 原槽。
    int per = base;

    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x8466b54 循环体首 `movl $0,-0x14`：member 显式置 0 再赋值（范式8 dead store）。
        CUser* member = 0;
        member = m_party->get_user(i);
        if (!member || !m_party->checkValidUser(i))
            continue;

        int amount = (member == user) ? total : per;
        int taxed = amount;                                // ORIG -0xc（可能翻倍）
        if (item.m_count != 0)
        {
            money2[i] = member->getCurCharacInvenR()
                            ->GetCreatureMgrR()->GetExtraGold(amount);
            taxed = taxed + taxed;  // ORIG 0x8466bf9：mov/add/mov 三段寄存器往返（4.4 对 *= 出 shl 内存形）
        }

        money1[i] = member->getCurCharacInvenW()->gain_money(
            taxed, (eMoneyAddReason)4, true, 0);
        if (item.m_count != 0)
            member->saveTaxMoneyForUpperMember(taxed);
        if (money1[i] < taxed)
        {
            if (money1[i] < 0)
                money1[i] = 0;
            member->SendMoneyFullReason(ENUM_MONEY_FULL_REASON_0,
                                        (unsigned long)taxed,
                                        (unsigned long)money1[i]);
        }
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)1, member, (unsigned int)money1[i]);
    }

}