// [b28c-4 P210] SendSignal 定义必须晚于全部 caller：4.4 对"已见定义"的 callee 按
// 其帧精确继承 outgoing（BreakJar 帧塌缩 0x1c、槽基 -0xc），ORIG 4.1 为保守帧
// 0x28/槽基 -0x14。定义后置即复现（探针 /tmp/l4/order.cpp T1 与 ORIG 0x0860b66c
// 逐条全等；同 TU 的 RankResult 等消费函数随之复查）。
void Statistics::SendSignal(int signal, void* data, int len) {}
