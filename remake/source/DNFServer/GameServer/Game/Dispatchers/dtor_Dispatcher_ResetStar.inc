// ==================== Dispatcher_ResetStar（ORIG 0x8140f6e）====================
// dispatch_sig：校验失败返回 0x5c0；read(0x14) → process → send_packet(0x244, 0xc)。
// process：res+0x04 = check_error(0)；resetStar 依赖未建模数据，标注推断。
Dispatcher_ResetStar::~Dispatcher_ResetStar() {}
