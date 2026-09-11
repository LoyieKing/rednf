// ==================== Dispatcher_UpgradeGage（ORIG 0x814020e）====================
// dispatch_sig：校验失败返回 0x446；read(0x18) → process → send_packet(0x23f, 0x14)。
// process：res+0x08=升级前 gageLevel、res+0x04=upgreadGage()、res+0x0c=升级后、
//          res+0x10=getGage()（真实偏移实现）。
Dispatcher_UpgradeGage::~Dispatcher_UpgradeGage() {}
