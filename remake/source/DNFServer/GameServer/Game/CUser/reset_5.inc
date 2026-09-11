// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// 定义置于本 TU（与 W7_UserOnlyC.cpp 中 CPacketVerifier::Init 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
void CPacketTimeChecker::reset()
{
    m_lastTime = 0;
    m_count = 0;
}