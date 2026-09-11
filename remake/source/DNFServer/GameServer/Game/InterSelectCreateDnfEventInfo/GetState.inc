// CEventCreateDnf::GetState 定义置于本 TU（与 CEventCreateDnf.cpp 中
// IsRewardCondition 调用者分离）：4.4 unit-at-a-time 对 TU 内可见 callee
// 塌缩 outgoing 帧（sub $0x4），ORIG 4.1 恒发 sub $0x18（探针 p6/p7 同族）。
int CEventCreateDnf::GetState()
{
    return static_cast<int>(m_state);
}