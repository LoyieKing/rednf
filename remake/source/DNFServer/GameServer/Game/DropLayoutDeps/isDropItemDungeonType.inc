bool CItem::isDropItemDungeonType(int idx)
{
    return m_drop.m_dungeonType[idx];       // ORIG 0x8539e24：movzbl 0xba(%edx,%eax)
}