void SetCollectDungeonStatistic(bool flag)
{
    // P0：ORIG `call; mov %eax,-0x4; mov -0x4,%eax` 指针局部 + 参数 home（同 IsCollectDungeonStatistic:130）。
    bool* p = GetCollectDungeonStatistic();
    *p = flag;
}