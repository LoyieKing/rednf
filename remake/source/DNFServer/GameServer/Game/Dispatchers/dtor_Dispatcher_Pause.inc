// ==================== Dispatcher_Pause（ORIG 0x8140a1c）====================
// dispatch_sig：state/charac 校验失败返回 0x52c；否则 read(0xe) → process →
// send_packet(0x242, 0xc) → 0。
// process：res+0x08 = req+0x0d（pause 标志）；check_error 恒 0；
//          Manager::pause(user, flag) → StageControl+0x04 ProcStage::pause。
Dispatcher_Pause::~Dispatcher_Pause() {}
