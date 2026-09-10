void CPacketRespondent::SendAccordingToPlace()
{
    m_autoSend = 0;
    if (m_user->get_state() == 5)
    {
        m_user->Send(*this);
    }
    else
    {
        // ORIG 0x83404b8：area 结果 `mov %eax,%ebx` 寄存器缓存（跨两次 call 存活），
        // vill 从 getCurCharacVill 返回值直转实参（无槽）。Lane6 探针 /tmp/l6/probe2.cpp：
        // 普通内联实参（R→L 自然求值，get_area 先求值入 ebx）与 ORIG 逐条一致；
        // 语句表达式 `({int r=f(); r;})` 反落槽（h2 形）。实参装载序 area→0x8、vill→0x4。
        Broadcast((int)(signed char)m_user->getCurCharacVill(),
                  m_user->get_area(false));
    }
