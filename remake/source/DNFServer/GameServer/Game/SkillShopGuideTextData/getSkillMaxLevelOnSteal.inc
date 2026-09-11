// ============================================================================
// getSkillMaxLevelOnSteal —— ORIG T 0x08a9af4f（0x53 字节）
// 报告 docs/class_func_reports/_lt_global_gt_/getSkillMaxLevelOnSteal.md
// 与 getSkillSpendSPOnSteal（SkillSlot.cpp）共用 .rodata 常量 0x8e2a180 = 100.0f：
//   f = penalty / 100.0f；r = (int)(sp * f)（fistpl 控制字改 0x0c → RC=11 截断
//   向零，与 C 浮点转整一致）；返回 std::max(1, r)。
// ============================================================================
int getSkillMaxLevelOnSteal(int sp, int penalty)
{
    float f = (float)penalty / 100.0f;
    int r = (int)((float)sp * f);
    return std::max(1, r);
}