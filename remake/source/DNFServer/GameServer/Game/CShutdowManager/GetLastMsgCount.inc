// ORIG 0x084ed1fe（_ZN15CShutdowManager15GetLastMsgCountEv，W）：
//   return *(unsigned int*)(this+0x04)。
unsigned int CShutdowManager::GetLastMsgCount()
{
    return static_cast<unsigned int>(m_field4);
}