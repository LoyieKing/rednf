// ==================== Dispatcher_BuyItem（ORIG 0x813fad0）====================
// dispatch_sig：校验失败返回 0x380；read(0x20) → process → send_packet(0x23d, 0x1c)。
// process：res 回显 req 全部字段；res+0x04 = check_error(0)；buyItem 依赖
//          CharacAdvanceAltarManager 内部 AdvanceAltarData（未建模），标注推断。
Dispatcher_BuyItem::~Dispatcher_BuyItem() {}
