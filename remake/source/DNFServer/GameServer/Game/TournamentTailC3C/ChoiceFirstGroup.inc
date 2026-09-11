// ---------------------------------------------------------------------------
// CBattle_Field::ChoiceFirstGroup @ 0x08303a1c
// 报告要点：先取 src 首元素 m_group 作基准（无条件解引用 begin()）；随后
// while(it!=end && it->m_group==group)：dst.push_back(*it)、it=src.erase(it)。
// ORIG 不清空 dst、无空表防护 —— 忠实保留。
void CBattle_Field::ChoiceFirstGroup(std::vector<TournamentInfo>& src,
    std::vector<TournamentInfo>& dst)
{
    std::vector<TournamentInfo>::iterator it = src.begin();
    const int group = it->m_group;
    // ORIG 条件序：先 it==end 测试（sete+test/je 退），后 m_group==group
    // 直分支 —— 拆嵌套消 4.4 对 && 右臂的 choose 物化。
    while (it != src.end())
    {
        if (it->m_group == group)
        {
            dst.push_back(*it);
            it = src.erase(it);
        }
        else
            break;
    }
}