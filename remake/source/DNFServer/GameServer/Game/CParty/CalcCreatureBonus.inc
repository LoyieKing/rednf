float CParty::CalcCreatureBonus(CUser* user)
{
    if (user->getCurCharacInvenR()->GetCreatureMgrR()->IsEquippedCreature())
        return 0.05f;
    return 0.0f;
