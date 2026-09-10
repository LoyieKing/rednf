float ServerParameterScript::GetPvPPenaltyRevision(int grade)
{
    std::map<int, int>::iterator it = m_mapb0.find(grade);
    // ORIG 0x08693918：命中正分支 → int 具名槽 fildl; flds 100.0f; fdivrp；
    // fld1 冷岛在尾。
    if (it != m_mapb0.end())
    {
        return (float)it->second / 100.0f;
    }
    return 1.0f;
