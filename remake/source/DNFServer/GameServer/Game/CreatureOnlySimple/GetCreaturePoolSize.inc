// 【objdump】ORIG _ZN13user_creature19GetCreaturePoolSizeEv T 0x08336c80：
// GetInstanceCreatureItemFactory()->GetSize(type=1)（creature）。
int GetCreaturePoolSize()
{
    return GetInstanceCreatureItemFactory()->GetSize(CREATURE_ITME_TYPE_CREATURE);
}