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
