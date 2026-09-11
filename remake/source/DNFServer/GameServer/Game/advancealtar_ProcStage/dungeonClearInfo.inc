// ----------------------------------------------------------------------------
// ORIG T 0x08133bd4（dungeonClearInfo.md 全文）：
//   playSec = startStageTime>=1 ? getCurSec()-startStageTime : 0；
//   names[0xff] 清零后 getMemberNames(user, names)；
//   cUserHistoryLog(user+0x79700).DungeonClearInfo(isLast, playSec, stageIndex,
//     0, 1, ENUM_DUNGEON_MODE 0, names, characData->m_field0)；
//   isLast 时 incrementEntranceData(advanceAltarstatistics_, idx, 0,1,0)
//     [CAdvanceAltarLog 未建模，跳过]。
// ----------------------------------------------------------------------------
void HistoryLog::dungeonClearInfo(CUser& user, StageControl& control, bool isLast)
{
    // b28d-solo-ProcStage：ORIG 0x08133bf0 起每次重发 control.getProcStage()
    //   （P48/P0 表达式内联，无 procStage 具名槽）；-0x1c=startStageTime、-0x20=playSec
    //   为 ORIG 具名局部（`movl $0,-0x20` 预清 + `sub -0x1c,%eax` 回写）；
    //   数组 -0x11f 用 P312 `={0}` 慢路（旧显式循环为逐字节形）；
    //   stageIndex/ridableData/hist 全内联（ORIG 无槽，ridableData 经 %ebx 保活）。
    // ORIG 槽序：-0x20=playSec（先声明，函数头预清 `movl $0,-0x20`）、
    //   -0x1c=startStageTime；分支为 then 后置（`cmpl $0,-0x1c; jg <then>`）⇒ 反向条件。
    long playSec = 0;
    const int startStageTime = control.getProcStage()->getStartStageTime();
    if (startStageTime <= 0)
    {
        playSec = 0;
    }
    else
    {
        playSec = static_cast<long>(
            GlobalData::s_systemTime_.getCurSec() - startStageTime);
    }

    char memberNames[255] = {0};              // ebp-0x11f 起 0xff 字节清零
    getMemberNames(&user, memberNames);

    user.m_historyLog.DungeonClearInfo(
        static_cast<int>(isLast), playSec,
        control.getProcStage()->getStageIndex(),
        0, 1, ENUM_DUNGEON_MODE_0, memberNames,
        control.getCharacData()->m_field0);

    if (isLast)
    {
        HistoryLog::advanceAltarstatistics_.incrementEntranceData(
            static_cast<unsigned int>(control.getProcStage()->getStageIndex()),
            0, 1, 0);
    }
}