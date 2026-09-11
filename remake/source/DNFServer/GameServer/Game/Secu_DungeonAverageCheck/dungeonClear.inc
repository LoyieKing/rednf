void Secu_DungeonAverageCheck::dungeonClear()
{
    // ORIG 0x82799d6：CheckHackTypeEndDungeon() + reset()。
    CheckHackTypeEndDungeon();
    reset();
}