// [b28d-solo-W9_SDCCommon] ORIG 0x0808d37a：P291/P235 union 双视角后偶槽单条
// scale-8、奇槽 scale-4、死槽 -0x14/-0x10、返回两段物化（fstps -0xc → 返回值
// 中转 -0x1c → flds）全等（探针 /tmp/sdc_p8.cpp p1 29/29）。
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
}