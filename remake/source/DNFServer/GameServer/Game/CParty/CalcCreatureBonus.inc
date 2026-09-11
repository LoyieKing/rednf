// CalcCreatureBonus：user 当前角色装备了 creature → 0.05f，否则 0.0f
float CParty::CalcCreatureBonus(CUser* user)
{
    if (user->getCurCharacInvenR()->GetCreatureMgrR()->IsEquippedCreature())
        return 0.05f;
    return 0.0f;
}