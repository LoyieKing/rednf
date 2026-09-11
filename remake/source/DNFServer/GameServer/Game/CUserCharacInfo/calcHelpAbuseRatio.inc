void CUserCharacInfo::calcHelpAbuseRatio(unsigned int param, bool flag)
{
    // [b28d-solo-CUserCharacInfo P232/P0] ORIG 帧 0x24 + 唯一 `mov %al,-0x24(%ebp)`：
    // 4.4 对 bool 形参恒 spill 到帧底槽（槽深=帧大小），探针 /tmp/p2d_probe.cpp
    // k1/k2/k3 实测 `sub $0x24; mov 0x10(%ebp),%eax; mov %al,-0x24(%ebp)` 与 ORIG
    // 逐条全等 ⇒ 死局部 char[0x20] 撑帧 0x24 即复现（旧数组赋值形两段已废）。
    char unused[0x20];
    (void)unused;
    (void)flag;
    (void)param;
}