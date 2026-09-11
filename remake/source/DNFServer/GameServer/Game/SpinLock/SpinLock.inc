// ---- SpinLock ----
// P210 前置：本 TU 的 Uncopyable ctor/dtor 定义放在 SpinLock C1/D1 之后，
// 使 4.4 对未可见 callee 采用保守帧（ORIG sub $0x18）。
SpinLock::SpinLock() : Uncopyable() { m_lock = 0; }
