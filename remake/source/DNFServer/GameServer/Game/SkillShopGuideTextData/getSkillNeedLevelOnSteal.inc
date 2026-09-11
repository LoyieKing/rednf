// ============================================================================
// getSkillNeedLevelOnSteal —— ORIG T 0x08a9aff5（0xe 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillNeedLevelOnSteal.md
// 汇编仅 lea (%edx,%eax,1) + ret：直接返回 param_1 + param_2。
// ============================================================================
int getSkillNeedLevelOnSteal(int level, int add)
{
    return level + add;
}