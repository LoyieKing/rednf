// ORIG 0x084ed1ea（_ZN15CShutdowManager29IncrementLastMsgResponseCountEv，W）：
//   ++*(int*)(this+0x08)。
void CShutdowManager::IncrementLastMsgResponseCount()
{
    ++m_field8;
}