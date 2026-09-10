void HistoryLog::enterDungeon(CUser& user, const ProcStage& procStage)
{
    // b28d-solo-ProcStage：ORIG 0x081339bb 是 4.1 的「对齐剥离 + rep stos + 尾字节」
    //   慢路（数组槽 -0x107 非 4 对齐）⇒ P312 形 `char buf[N]={0}` + 前置死局部
    //   （1 字节 pad 把数组顶到非对齐槽）；旧显式循环为逐字节形。
    char memberNames[255] = {0};             // ebp-0x107 起 0xff 字节清零
    getMemberNames(&user, memberNames);

    // P0：ORIG 无 difficulty/stageIndex/hist 具名槽——difficulty 经 %ebx 跨
    //   getStageIndex 调用保活（`mov %eax,%ebx`）、stageIndex 留 %eax、
    //   hist 由 `lea 0x79700(%edx),%ecx` 内联 ⇒ 全部表达式内联。
    user.m_historyLog.EnterDungeon(procStage.getStageIndex(),
                                   procStage.getStageDifficulty(), 1,
                                   memberNames, 2, 0);

    HistoryLog::advanceAltarstatistics_.incrementEntranceData(
        static_cast<unsigned int>(procStage.getStageIndex()), 1, 0, 0);
