float expert_job::CAlchemist::GetItemUpgradeSuccessRate()
{
    static const float divisor = 9840.0f;  // ORIG .rodata 0x08cbfa3c
    return (float)g_alchemistInfo[0] / divisor;   // 合并后 int[2]；低半区 [0] 为原标量
