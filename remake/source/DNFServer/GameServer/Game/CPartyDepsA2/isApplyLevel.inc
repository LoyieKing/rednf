// ============================================================================
// ConditionLevelChkDungeon::isApplyLevel —— ORIG T 0x08913a62
//   （报告 ConditionLevelChkDungeon/isApplyLevel.md）：m_applyLevel <= level。
// ============================================================================
bool ConditionLevelChkDungeon::isApplyLevel(int level)
{
    if (!(m_applyLevel > level))
    {
        return true;
    }
    return false;
}