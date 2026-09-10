Dispatcher_CompleteLoadPvP::~Dispatcher_CompleteLoadPvP() {}
// ORIG 0x082252fe（报告 Dispatcher_CompleteLoadPvP/dispatch_sig.md）：
// state<=1 → LineFunc(0xe005)；matching 非空 → IMatch +0x54 vcall；
// room/matching 为空 → 直接 return 0（无 CAssaultMgr 回退分支）。
// [b28c-32] verdict=NEAR：P214① 正向包裹+双 return 0（共享 je 尾）；余 N7（42=42）。
