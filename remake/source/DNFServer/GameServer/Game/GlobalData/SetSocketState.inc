// P119：ORIG W 弱符号（0x0857f444/0x0857f456/0x08471e08），inline 内联不发射
// 独立符号 → 本属主 TU 外联定义。GetSocketState 返回 int（ORIG mov 0x30e00,%eax）。
void CPCRoomServerProxy::SetSocketState(ENUM_SOCKET_STATE state)
{
    m_socketState = state;
}