// InterDispatcher / PacketDispatcher / TimerDispatcher —— 权威类型来自
// InterDispatcher.h / PacketDispatcher.h / TimerDispatcher.h（本 TU 顶部 include）。
// 真实构造/析构/注册表在各自 TU（ORIG C1 0x84bbafa / 0x8590a2e / 0x863120a）。
void CREATE_InterDispatcher()   { GlobalInstance<InterDispatcher>::inst(); }
