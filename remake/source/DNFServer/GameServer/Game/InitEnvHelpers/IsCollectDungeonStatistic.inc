bool IsCollectDungeonStatistic(void)
{
    // ORIG 0x08484a51 区：call 后 `mov %eax,-0x4(%ebp); mov -0x4(%ebp),%eax` 槽
    // 往返（P0 具名局部）+ sub 0x10 帧。具名指针局部复现。
    bool* p = GetCollectDungeonStatistic();
    return *p;   // ORIG movzbl (%eax),%eax
