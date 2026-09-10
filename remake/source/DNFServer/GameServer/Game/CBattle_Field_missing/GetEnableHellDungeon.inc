bool* CBattle_Field::GetEnableHellDungeon()
{
    // mov $0x93701ec,%eax; ret（POD 无 guard 的函数局部
    // static，V 符号 _ZZN13CBattle_Field20GetEnableHellDungeonEvE19enable_hell_dungeon）。
    static bool enable_hell_dungeon;
    return &enable_hell_dungeon;
