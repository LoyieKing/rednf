DisPatcher_SetTradeState::~DisPatcher_SetTradeState() {}
// ORIG 0x081f6232 T _ZN24DisPatcher_SetTradeState12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state==3 且 get_byte(state) 成功 → GetTradeSpace(idx) 池取；空 → SendCmdErrorPacket
// (0x1a,1)；is_empty → logFormat(0x82f1) + PutTradeSpace；check_trade_possibility 假 →
// cancel_trade + PutTradeSpace；真 → change_trade_state(user, (char)state)；返回 0。
