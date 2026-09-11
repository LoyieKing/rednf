// ORIG T 0x8892445
// ORIG T 0x8892445（0x26 字节，objdump 转写：报告缺失）
// if (sum == 0) return 0; *sum ^= key; return *sum;
int finishSkillChecksum(int* sum, int key)
{
    if (sum == 0)
    {
        return 0;
    }
    *sum = *sum ^ key;
    return *sum;
}