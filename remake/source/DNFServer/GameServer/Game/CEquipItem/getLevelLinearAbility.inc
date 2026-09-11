// ORIG 0x898c79d：线性插值
// [b28d-solo-CEquipItem] 逐条还原（ORIG 0x0898c79d；帧 0x68）：
//   ① vSpan/vLevel/vDiff 三个 int 槽按 ORIG 顺序先算（vDiff=d-c 在 x87 段之前，槽 -0x24）；
//   ② ratio 的钳位是 `ratio <= 0.0`（ORIG `fucompp;fnstsw;test $0x45,%ah;sete;test;je`），
//      不是旧写的 `ratio == 0.0`（后者 4.4 产 fucomip+jne/jp 双跳）；
//   ③ `double diff = (double)vDiff;` 具名局部复现 ORIG `fildl -0x24;fstpl -0x50`。
//   x87 比较 lowering 需 -march=i486 才出 ORIG 的 fnstsw/sahf 形（P230 补；已上报 Main 路由）。
// [b28e 三形实测] 帧/槽：①无 pad ⇒ `sub $0x58`（ORIG 0x68）、diff -0x38、floor 临时 -0x40；
//   ②pad 置首（本形）⇒ `sub $0x68` = ORIG、diff -0x38、临时 -0x50；③`double diff` 迁到
//   三个 int 之后 ⇒ 全槽整体平移（vSpan -0x34 等）、更劣已回退。ORIG 形（diff -0x50、
//   临时 -0x40、帧 0x68）要求 4.1 的「具名 double 深槽 / 临时浅槽」方向，4.4 恒反向
//   （具名 -0x38 / 临时 -0x50），与 0x.. 帧差无关 ⇒ 残差 = 两 double 槽互换，N82附/P112 族。
int getLevelLinearAbility(int a, int b, int c, int d, int level)
{
    // [b28e 帧对齐] ORIG 帧 `sub $0x68`（4.1 对 x87 段多预留 0x10）；无此局部时
    // 我方 0x58（帧差 0x10）。按 P232 补未使用局部后帧 **0x68 = ORIG**。
    // [b2c 试形 A·回滚] `double diff;` 提到 framePad 之前（P339 声明序 ⇒ 最深槽）
    // 实测读数不变（diff 仍 -0x38、floor 临时仍 -0x50、near=1）⇒ 4.4 具名 double
    // 槽与声明序无关，按纪律回滚。
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
}