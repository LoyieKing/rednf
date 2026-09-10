void AvatarRechargeServer::CalcDurabilityChangeEquip(CUser* user, int equipSlot)
{
    if (user->get_state() != 5)
        return;
    if (user->GetParty() == 0)
        return;
    if (!user->GetParty()->IsUseFatigueDungeon())
        return;
    if (user->GetParty()->CheckMapClear())
        return;

    std::vector<int> rates;            // ORIG 栈 vector（空构造即传入）
    if (-1 < equipSlot && equipSlot < 10)
    {
        while (!CalcDurabilityDetail(user, equipSlot, rates))
            break;
    }
