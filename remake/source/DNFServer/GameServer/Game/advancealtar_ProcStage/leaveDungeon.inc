void HistoryLog::leaveDungeon(CUser& user, const ProcStage& procStage)
{
    // b28d-52：ORIG 0x08133abc `mov $0x40,%edx; rep stos` = 0x100 字节零初始化
    // （P214③：char buf[N]={0} 复现 4.1 rep stos；旧 255 + 显式循环为逐字节形）。
    char memberNames[0x100] = {0};
    getMemberNames(&user, memberNames);

    // P0：ORIG 无 difficulty/stageIndex 槽（difficulty 入 %ebx 跨调用保活）⇒ 实参内联。
    user.m_historyLog.LeaveDungeon(procStage.getStageIndex(),
                                   procStage.getStageDifficulty(), memberNames, 0);
