// ORIG T _Z22getSkillSpendSPOnStealii（0x8a9afa2，0x53 字节）：
// f = penalty / 100.0f（.rodata 0x8e2a180 = 0x42c80000）；r = (int)(sp * f)
// （fistpl，控制字 chop 截断向零）；返回 std::max(1, r)。
int getSkillSpendSPOnSteal(int sp, int penalty)
{
    float f = (float)penalty / 100.0f;
    int r = (int)((float)sp * f);
    return std::max(1, r);
}