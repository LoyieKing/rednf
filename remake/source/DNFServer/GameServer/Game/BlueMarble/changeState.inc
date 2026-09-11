void BlueMarble::changeState(BlueMarbleState::T state)
// 原 int；ORIG 0x080d9202 尾 pop;ret 无 %eax 置值 ⇒ void（mangled 无返回类型）
{
    m_state = state;
}