float SDCActiveStatusSyncCommon::getActiveStatusEtcParameter(int idx, int level,
                                                             int j)
{
    // ORIG 0x0808d446/0x0808d44d：entry 双 dead init 槽 -0x14=0/-0x10=1（P31）；
    // 返回两段物化（fstps -0xc → mov %eax,-0x1c → flds -0x1c，P0 双具名局部）。
    // [N82 封顶] 槽/指令已全对齐；唯一残差帧 0x30(ORIG 16B 对齐进位) vs 0x24(4.4
    // 紧凑)，P190 double pad 实测超调 0x38，封顶。
    int lo = 0;
    int hi = 1;
    (void)lo; (void)hi;
    // ORIG 0x0808d43e：结果先落具名 r 槽 -0xc，再经 eax 拷独立返回槽 -0x1c（P0 双局部）。
    float r = GetUniformVelocityFloatSDC(
        ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2],
        ACTIVESTATUS_ETC_PARAMETERS[idx][j * 2 + 1],
        level - 1, ACTIVESTATUS_MAXLEVEL - 1);
    return r;
