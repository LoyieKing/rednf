// 【objdump】ORIG _ZN13user_creature22GetCreatureEggPoolSizeEv T 0x08336c63：
// GetInstanceCreatureItemFactory()->GetSize(type=0)（egg）。
int GetCreatureEggPoolSize()
{
    return GetInstanceCreatureItemFactory()->GetSize(CREATURE_ITME_TYPE_EGG);
}