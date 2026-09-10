int CDisjointer::OnRepairDisjointMachine(CUser* user)
{
    int newEndurance = 0;
    int endurance =
        ((CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance();
    int grade =
        ((CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_grade();
    int currentPrice =
        GetScript()->GetDisjointMachineRepairCost(grade + 1, endurance);

    if (user->getCurCharacMoney() < currentPrice)
    {
        // 现金不足：以全部金钱换算，返回基础值 + 当前耐久（出参回写现价）。
        newEndurance =
            GetScript()->GetDisjointMachineRepair(
                grade + 1, user->getCurCharacMoney(), currentPrice) +
            endurance;
    }
    else
    {
        stEnduranceRepairCost* cost =
            GetScript()->_GetEnduranceRepairCost((unsigned)(grade + 1));
        if (cost != 0)
        {
            newEndurance = cost->m_field4;   // 满值槽
        }
    }

    if (newEndurance < 1)
    {
        return 0x16;
    }

    user->getCurCharacInvenW()->use_money(currentPrice, (eMoneySubReason)0x20, true);
    ((CCharacExpertJob*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
        ->set_disjoint_machine_endurance(newEndurance);

    PacketGuard packet;
    packet.put_header(1, 0xf0);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacMoney());
    packet.put_int(
        ((CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
