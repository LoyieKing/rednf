// ---------------------------------------------------------------------------
// SigServerState::tagData C1/C2 —— ORIG W 0x0818670c：
//   mov 0x8(%ebp),%eax; add $0x8,%eax; call AradServerStateMessage::Ctor(0x8186652)
// 即对成员 m_msg（tag 内 +0x08）执行 AradServerStateMessage 默认构造；
// C++ 中空体用户构造函数自动完成同样的成员对象构造。
// ---------------------------------------------------------------------------
SigServerState::tagData::tagData() {}
