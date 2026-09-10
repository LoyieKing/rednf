float SDCActiveStatusSyncCommon::getActiveStatusPower2(int idx, int level)
{
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    return GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER2.m_pair[idx].m_lo,
                                      ACTIVESTATUS_POWER2.m_flat[idx * 2 + 1],
                                      level - 1, ACTIVESTATUS_MAXLEVEL - 1);
