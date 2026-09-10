bool CCreature::Equip(CUser* user)
{
    if (user->is_fighting())
        SetState(user, CREATURE_STATE_1);
    else
        SetState(user, CREATURE_STATE_3);
    return true;
}
