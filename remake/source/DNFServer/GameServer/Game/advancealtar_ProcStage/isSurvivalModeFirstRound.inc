// ORIG T 0x0812f70c：stageType==1 && survivalFirst!=0。
bool ProcStage::isSurvivalModeFirstRound() const
{
    // b28d-solo-ProcStage：P0/N96 清绑（原 `register bool r __asm__("al")` 违禁）。
    // ORIG 0x0812f70d 汇聚值仅存 %eax/%al（无栈槽往返）⇒ 用 P226 条件向语句表达式
    // （`if (({ A && B; }))`，内不含具名变量）复现短路物化岛 + 零槽。
    if (({ getStageType() == 1 && m_survivalFirst != 0; }))
        return true;
    return false;
}