// 【objdump】ORIG _ZN13user_creature25GetCreaturePoolRemainSizeEv T 0x08336cba：
// GetInstanceCreatureItemFactory()->GetRemainSize(type=1)。
int GetCreaturePoolRemainSize()
{
    return GetInstanceCreatureItemFactory()->GetRemainSize(
        CREATURE_ITME_TYPE_CREATURE);
}