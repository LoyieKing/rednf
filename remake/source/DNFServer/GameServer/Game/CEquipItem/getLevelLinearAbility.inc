int getLevelLinearAbility(int a, int b, int c, int d, int level)
{
    // [b28e 帧对齐] ORIG 帧 `sub $0x68`（4.1 对 x87 段多预留 0x10）；无此局部时
    // 我方 0x58（帧差 0x10）。按 P232 补未使用局部后帧 **0x68 = ORIG**。
    char framePad[0x10];
    (void)framePad;
    double diff;                                    // ORIG：声明在函数头（深槽 -0x50）
    int vSpan = b - a;                              // ORIG：先求除数（-0x2c）
    int vLevel = level - a;                         // ORIG：再求被除数（-0x28）
    int vDiff = d - c;                              // ORIG：-0x24（在 x87 段之前）
    double ratio = (double)vLevel / (double)vSpan;
    // ORIG `fucompp;fnstsw;test $0x45,%ah;sete;test;je` = !(x>0) 钳回 0；
    // 掩码 0x45（C0|C2|C3 全 0 = 严格大于）对应源码 `x < 0.0`；`x <= 0` 产 `test $0x5`。
    if (ratio < 0.0)
        ratio = 0.0;
    if (ratio > 1.0)
        ratio = 1.0;
    diff = (double)vDiff;                           // ORIG：-0x50
    // ORIG 无具名 result 局部：floor 结果留 st0 直接续算（`fldl const;fdivrp;fmull -0x50`）。
    double result2 = floor(ratio * 1000000.0 + 0.5L) / 1000000.0 * diff;
    int out = (int)((double)c + result2);
    if (out < 0)
        out = 0;
    return out;
