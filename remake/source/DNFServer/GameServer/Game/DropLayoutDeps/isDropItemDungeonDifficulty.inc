bool CItem::isDropItemDungeonDifficulty(int idx)
{
    return m_drop.m_difficulty[idx];        // ORIG 0x8539e10：movzbl 0xb5(%edx,%eax)
