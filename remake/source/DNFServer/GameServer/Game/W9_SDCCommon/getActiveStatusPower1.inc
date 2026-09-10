float SDCActiveStatusSyncCommon::getActiveStatusPower1(int idx, int level)
{
    // P31/P184：ORIG entry 双死 init 槽 -0x14=0/-0x10=1（照抄）；
    // float 具名 v -0xc + return v 的返回值中转槽 -0x1c（P0 双槽）。
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    float v = GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER1.m_pair[idx].m_lo,
                                         ACTIVESTATUS_POWER1.m_flat[idx * 2 + 1],
                                         level - 1, ACTIVESTATUS_MAXLEVEL - 1);
    return v;
