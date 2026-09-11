// 【objdump】ORIG _ZN13user_creature28GetCreatureEggPoolRemainSizeEv T 0x08336c9d：
// GetInstanceCreatureItemFactory()->GetRemainSize(type=0)。
int GetCreatureEggPoolRemainSize()
{
    return GetInstanceCreatureItemFactory()->GetRemainSize(
        CREATURE_ITME_TYPE_EGG);
}