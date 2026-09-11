// ============================================================================
// GetExtracterType —— ORIG T 0x084a2904（报告 GetExtracterType.md）
//   专业职业类型 → 提取器索引：2→0(Alchemist)/4→2(DollController)/1→1(Enchanter)/其它→3
// ============================================================================
int GetExtracterType(ENUM_EXPERT_JOB_TYPE job)
{
    switch (job)
    {
    case (ENUM_EXPERT_JOB_TYPE)2:
        return 0;
    case (ENUM_EXPERT_JOB_TYPE)4:
        return 2;
    case (ENUM_EXPERT_JOB_TYPE)1:
        return 1;
    default:
        return 3;
    }
}