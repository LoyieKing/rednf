user_creature::CCreatureItemFactory* user_creature::GetInstanceCreatureItemFactory()
{
    static user_creature::CCreatureItemFactory s_instance;
    return &s_instance;
