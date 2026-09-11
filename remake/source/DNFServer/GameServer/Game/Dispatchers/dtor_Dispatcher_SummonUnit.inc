// ==================== Dispatcher_SummonUnit（ORIG 0x81404a8）====================
// dispatch_sig：校验失败返回 0x496；read(0x18) → process → send_packet(0x240, 0x14)。
// process：res+0x08=req+0x0d、res+0x0c=req+0x10；check_error 0；若 gageLevel 匹配
//   summonSlotUnit（真实偏移实现）；res+0x10 = getGage()。
Dispatcher_SummonUnit::~Dispatcher_SummonUnit() {}
