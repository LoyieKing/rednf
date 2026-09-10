Dispatcher_ScanBotByDll::~Dispatcher_ScanBotByDll() {}
// b28d-solo-Dispatchers verdict=AE(routed) [P310/P209附] ORIG 0x08264538 orig/ours=20/23
//   ORIG 两段形 `mov -0x4(%ebp),%eax; cmp $0x351,%eax; jle`（P209 附：signed 局部
//   被 4.4 折 cmpl mem,imm）。翻案 = 局部 unsigned + 使用点 `(int)wv` 比较
//   （P310 变体；探针 /tmp/b28d_disp/probe_wv.cpp k1 逐条命中两段+jle 方向）。
