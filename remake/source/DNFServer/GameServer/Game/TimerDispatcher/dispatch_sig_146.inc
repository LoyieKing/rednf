int Timer_DungeonStatisticEntrance_Hour::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863b9da: GetInstanceDungeonEntranceLog()->InsertDungeonEntrance_Hour(); RegistNextTimer(); return 1
    ((CDungeonEntranceLog*)GetInstanceDungeonEntranceLog())->InsertDungeonEntrance_Hour();
    RegistNextTimer();
    return 1;
