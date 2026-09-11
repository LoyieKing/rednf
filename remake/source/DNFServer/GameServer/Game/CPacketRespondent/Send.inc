// ORIG Send @ 0x083403e0：+0x1c=0；CUser::Send(*this) 结果透传。
// [b28d-solo-CPacketRespondent P248档①] ORIG 尾 `call CUser::Send; leave; ret` 无 movzbl
// ⇒ 与 callee 同型 bool（CUser::Send 返回 bool，__PRETTY_FUNCTION__ 串 "bool CUser::Send"）。
// 声明 int 时 4.4 插 `movzbl %al,%eax`（4.1 不插）——改 bool 即 identical。
bool CPacketRespondent::Send()
{
    m_autoSend = 0;
    return m_user->Send(*this);
}