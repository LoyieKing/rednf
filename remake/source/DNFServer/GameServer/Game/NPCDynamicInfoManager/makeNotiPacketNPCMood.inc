void CNPCDynamicInfoManager::makeNotiPacketNPCMood(PacketGuard& guard) const
{
    guard.put_header(0, 0xc3);
    int index = guard.get_index();
    short count = 0;
    guard.put_short(0);
    std::map<unsigned int, CNPCDynamicInfo*>::const_iterator it = m_map.begin();
    std::map<unsigned int, CNPCDynamicInfo*>::const_iterator endIt = m_map.end();
    while (it != endIt) {
        int mood = it->second->getMood();
        it->second->makeNotiPacketCurMood(guard);
        ++count;
        ++it;
    }
    guard.put_short(index, count);
    guard.finalize(true);
}