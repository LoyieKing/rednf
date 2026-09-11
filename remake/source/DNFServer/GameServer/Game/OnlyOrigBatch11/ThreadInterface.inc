// ORIG T 0x08630b38：仅 vptr store + ret（P57；m_tid/m_tidCopy 不在此层初始化）。
// 定义必须与派生线程类 TU（GlobalData.cpp 等）分离：4.4 对同 TU 可见空体 ctor 做
// non-throw 推断，导致 GlobalData::Init 的 `new DispatchThread` 等块省略 ORIG 实有
// 的 delete+Unwind cleanup（0x0829a4f3 等 pad，实测 probe 确认）。
ThreadInterface::ThreadInterface() {}
