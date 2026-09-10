Dispatcher_CheckAssaultMotionHack::~Dispatcher_CheckAssaultMotionHack() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112+N122+N197②] ORIG 0x08221f3c orig/ours=213/217
//   [S162] 已修：inPowerWar 直接初始化 → if/else 分支写常量（ORIG 0x0822204e 形，P194）；
//   half 具名 bool → 条件表达式直用（P226）；overCnt/overRem 预清两句式（P198①）。
//   残余：①`!inPowerWar` 4.4 恒 xor 物化 vs ORIG `cmpb $0,-0x11`（N122）；
//   ②overCnt 实参 4.4 直压栈 vs ORIG 升格 %ebx 跨 call（N82附/P197② 寄存器对调）；
//   ③then/else 块物理序差（N104 族）。语义已对齐。
//   首差: movl   $0x0,-0x10(%ebp) | mov    -0x18(%ebp),%eax | ... （S162 后首差位=88）
