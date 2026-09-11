// ORIG 0x082a6f70（_ZN15CShutdowManager14EnableShutdownEb，W）：
//   *(bool*)this = on。
void CShutdowManager::EnableShutdown(bool on)
{
    m_flag = on;
}