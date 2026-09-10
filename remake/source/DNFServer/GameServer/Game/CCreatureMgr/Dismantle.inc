bool CCreature::Dismantle(CUser* user)
{
    SetState(user, CREATURE_STATE_2);
    return true;
}
