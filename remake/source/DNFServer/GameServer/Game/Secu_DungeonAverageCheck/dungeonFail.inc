void Secu_DungeonAverageCheck::dungeonFail()
{
    // ORIG 0x82799f4：CheckHackTypeEndDungeon() + reset()。
    CheckHackTypeEndDungeon();
    reset();
}