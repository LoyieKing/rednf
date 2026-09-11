// [b28d-solo-W9_SDCCommon] ORIG 0x0808d3dc：union 双视角 + 双死 init 槽
// -0x10/-0xc + **直接 return 调用表达式**（无具名 v）⇒ 4.4 返回值中转槽 -0x1c
// 自拷贝形（fstps -0x1c; mov -0x1c,%eax; mov %eax,-0x1c; flds -0x1c），与 ORIG
// 30/30 全等（探针 /tmp/sdc_p8.cpp p6；注意 Power1 是具名 v 的 p1 形，两函数
// 原码结构不同）。
float SDCActiveStatusSyncCommon::getActiveStatusPower2(int idx, int level)
{
    int deadA = 0;
    int deadB = 1;
    (void)deadA; (void)deadB;
    return GetUniformVelocityFloatSDC(ACTIVESTATUS_POWER2.m_pair[idx].m_lo,
                                      ACTIVESTATUS_POWER2.m_flat[idx * 2 + 1],
                                      level - 1, ACTIVESTATUS_MAXLEVEL - 1);
}