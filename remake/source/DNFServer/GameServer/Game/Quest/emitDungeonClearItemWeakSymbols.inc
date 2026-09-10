void emitDungeonClearItemWeakSymbols()
{
    DungeonClearItem item;   // C1/C2（调 clear）
    item.clear();            // clear
    item.~DungeonClearItem();  // D1/D2
