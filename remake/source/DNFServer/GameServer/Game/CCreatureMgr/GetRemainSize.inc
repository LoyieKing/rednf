// ORIG _ZN13user_creature20CCreatureItemFactory13GetRemainSizeENS_18CREATURE_ITME_TYPEE
// W 0x0833f510：同 GetSize 分派，调 DynamicPool::Remain()。
int user_creature::CCreatureItemFactory::GetRemainSize(
    user_creature::CREATURE_ITME_TYPE type)
{
    if (type == user_creature::CREATURE_ITME_TYPE_CREATURE)
        return m_creaturePool.Remain();
    if (type == user_creature::CREATURE_ITME_TYPE_EGG)
        return m_eggPool.Remain();
    return 0;
}
