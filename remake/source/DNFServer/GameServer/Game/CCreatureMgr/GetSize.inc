// ORIG _ZN13user_creature20CCreatureItemFactory7GetSizeENS_18CREATURE_ITME_TYPEE
// W 0x0833f546：type==1 → m_creaturePool.Size()；type==0 → m_eggPool.Size()；
// 其余返回 0。
int user_creature::CCreatureItemFactory::GetSize(user_creature::CREATURE_ITME_TYPE type)
{
    if (type == user_creature::CREATURE_ITME_TYPE_CREATURE)
        return m_creaturePool.Size();
    if (type == user_creature::CREATURE_ITME_TYPE_EGG)
        return m_eggPool.Size();
    return 0;
}
