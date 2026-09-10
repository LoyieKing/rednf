int _NS_PacketIntegrity(CUser* user, PacketBuf& packet)
// [b28c-L23 N(P75 实参装载时刻) 残余 4 行] PIAddServerHackCnt 的 0x8/0x4(%esp)
// 两实参：ORIG @0x08274... 先载 value(-0x10)→%eax→0x8(%esp)，再载 hackType(-0xc)→
// %eax→0x4(%esp)（严格 R2L、同一寄存器复用）；4.4 因 hackType 是刚 call 完的结果
// 而先取 %eax、value 走 %edx（P75 跨版本装载时刻调度，压栈顺序/调用约定/语义全同）。
// 117=117 条数与助记符序列一致。
{
    unsigned char a, b, c, d;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;
    if (!packet.get_byte(c))
        return 3;
    if (!packet.get_byte(d))
        return 4;
    if (user == 0)
        return 4;

    int value = d % 10 + (b % 10) * 10 + (a % 10) * 100;
    // [b28d-solo-GameSecurity] 具名枚举局部 + 实参直传（无调用点 cast）复现 ORIG
    // 严格 R2L 逐槽装载 `mov value,%eax; mov %eax,0x8(%esp); mov hackType,%eax;
    // mov %eax,0x4(%esp)`（探针 /tmp/pi_probe.cpp probe4）；int 局部 + 实参处
    // static_cast 会令 4.4 先载第二实参到 %eax 并借 %edx 中转（P75 假象）。
    WongWork::ENUM_HACKTYPE hackType = static_cast<WongWork::ENUM_HACKTYPE>(
        PI_GetHackType(static_cast<short>(c)));
    PIAddServerHackCnt(user, hackType, value);
    return 0;
