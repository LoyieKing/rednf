// ==================== Dispatcher_SetSlot（ORIG 0x813ff3e）====================
// dispatch_sig：校验失败返回 0x3fc；read(0x24) → process → send_packet(0x23e, 0x1c)。
// process：res 回显 req（含 _SetSlotData 12B）；slotType 非 1/2 → error=2；
//          setSlot 依赖未建模 AdvanceAltarData，标注推断。
Dispatcher_SetSlot::~Dispatcher_SetSlot() {}
