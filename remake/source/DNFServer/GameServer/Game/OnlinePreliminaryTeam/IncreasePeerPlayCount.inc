// ORIG T 0x08586208：find；命中→flag=1、count++；未命中且 size>999 →
// cMyTrace 报警后直接 return（不 alter）；否则插入 PlayCountInfo(1,true)；
// 末尾 alter()。
// [b28d-solo-OnlinePreliminaryTeam] P222 正向包裹（命中块在前、`je` 落 miss）
// + P227 全表达式临时链（ORIG 0x085862c5 先 size() 入 %ebx 跨 ctor 保活）。
// ORIG 0x08586208
void COnlinePreliminaryMemberMatchList::IncreasePeerPlayCount(
    int serverId, unsigned int uniqueId)
{
    CharacterInServer key(serverId, uniqueId);
    std::map<CharacterInServer, PlayCountInfo>::iterator it =
        m_peerMap.find(key);
    if (it != m_peerMap.end())
    {
        it->second.m_flag = 1;
        it->second.m_count += 1;
    }
    else
    {
        if (m_peerMap.size() > 999)
        {
            // ORIG __PRETTY_FUNCTION__ 行号 0x65（101）
            cMyTrace(__PRETTY_FUNCTION__, 0x65, 0)(
                "Online Preliminary peer play count map size over! %d",
                static_cast<unsigned int>(m_peerMap.size()));
            return;
        }
        m_peerMap.insert(std::make_pair(CharacterInServer(serverId, uniqueId),
                                        PlayCountInfo(1, true)));
    }
    alter();
}