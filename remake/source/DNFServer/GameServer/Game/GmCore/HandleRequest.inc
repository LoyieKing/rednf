void IGameMaster::HandleRequest(CCommand* cmd)  // 0x082a7ed6
{
    // b26 NEAR 封顶:虚调用 vtable 项寄存器 ORIG 4.1 用 %ecx、4.4 恒 %eax
    // (负例 N7,临时/引用/限定名均不可翻),余逐条相等。
    if (m_next != 0)
    {
        m_next->HandleRequest(cmd);             // ORIG：虚调用 vtable[0]
    }
}