bool QuestList::check_clear(int questIdx, const WongWork::CQuestClear& clear)
{
    // [b28d-32] ORIG 0x08355284：`jle +0x5c`（>0x752f 走 trace 块 fall-through，
    // ≤ 直跳 body）。正向 if-early-return 形复现，goto 尾标签会多一条对齐 nop。
    if (questIdx > 0x752f)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x152f, 5);
        trace("[%s][%d]",
              __PRETTY_FUNCTION__,
              0x152f);
        return 0;
    }
    if (clear.isClearedQuest(questIdx))
    {
        return 0;
    }
    return 1;
