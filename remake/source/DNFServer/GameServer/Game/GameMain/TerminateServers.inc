void TerminateServers();    // ORIG T 0x082a21f9（本文件尾部定义）

// [b28d-28] DIFF | ORIG 0x0829f28b | P167 CLEAR（48/48）；N82 槽/寄存器分配差
// （我方多 28 条）。
// [b28d-solo-GameMain] AE | 121/122 | p167 48/48 CLEAR | 修法：CPacketTracerInstance()/
// G_CEnvironment()/G_GameWorld() 全表达式直用（P0，消 tracer/env/world 具名局部）+
// P227 cMyTrace 全表达式临时链（userCount 跨 ctor %ebx 保活）。
