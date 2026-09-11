// ORIG 0x084ed20a（_ZN15CShutdowManager23GetLastMsgResponseCountEv，W）：
//   return *(unsigned int*)(this+0x08)。
unsigned int CShutdowManager::GetLastMsgResponseCount()
{
    return static_cast<unsigned int>(m_field8);
}