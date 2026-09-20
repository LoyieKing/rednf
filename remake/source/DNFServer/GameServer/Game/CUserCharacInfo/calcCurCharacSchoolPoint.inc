// ORIG 0x08655bce 有符号除 `fatigue/10` 魔数段 `sar $0x1f,%eax;sub %eax,%edx` 直落 %edx；
// 4.4 经 %ecx 中转 `mov %edx,%ecx;sub %eax,%ecx;mov %ecx,%eax;mov %eax,%edx`（N7/N18a
// 寄存器路由），除运算与数组索引逐段全等，分支 je/jbe 落点合规，语义等价。
void CUserCharacInfo::calcCurCharacSchoolPoint(int fatigue)
{
    static const int point[24] = {
        0, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 5
    };
    static const int pointSize = sizeof(point) / sizeof(point[0]);
    unsigned short used = getCurCharacUsedFatigue();
    // [b28d-L14 实证] 右操作数显式 (int) 化消 4.4 的 int→unsigned 物化四连
    // （探针 /tmp/l14_probe4.cpp c4 与 ORIG 0x08655bce 逐条全等：sub %eax,%edx 直落）。
    if (fatigue / 10 == (int)(used / 10u))
        return;
    unsigned int index = used / 10u;
    if (index > 0x17)
        index = 0x17;
    m_selected->m_schoolPoint += point[index];
}