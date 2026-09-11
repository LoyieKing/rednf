// ORIG T 0x085884b6：find；命中→flag=1、count++；未命中且 size>999 →
// cMyTrace 报警后直接 return（不置 0x3d）；否则插入 PlayCountInfoEx(1,true)；
// 最后 m_hasLoseFlag(+0x3d)=1。
// [b28d-solo-OnlinePreliminaryTeam] P222 正向包裹（ORIG 命中块在前、`je` 落
// miss 块）+ P227 全表达式临时链（ORIG 先 size() 并 %ebx 跨 ctor 保活、
// 再构造 cMyTrace）。ORIG 0x085884b6
void COnlinePreliminaryTeam::IncreaseEachOtherTeamPlayCount(int teamId)
{
    std::map<int, PlayCountInfoEx>::iterator it = m_playCountMap.find(teamId);
    if (it != m_playCountMap.end())
    {
        it->second.m_flag = 1;
        it->second.m_count += 1;
    }
    else
    {
        if (m_playCountMap.size() > 999)
        {
            // ORIG __PRETTY_FUNCTION__ 行号 0xca（202）
            cMyTrace(__PRETTY_FUNCTION__, 0xca, 0)(
                "Online Preliminary each other team play count map size over! %d",
                static_cast<unsigned int>(m_playCountMap.size()));
            return;
        }
        m_playCountMap.insert(std::make_pair(teamId, PlayCountInfoEx(1, true)));
    }
    m_hasLoseFlag = 1;
}