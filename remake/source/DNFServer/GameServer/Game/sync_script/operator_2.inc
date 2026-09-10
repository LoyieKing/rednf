void CStringMaker::operator()(SkillUseItem v)         // 0x08626d98 "%d,%d,%d"
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    sprintf(num, "%d,%d,%d", v.m_index, v.m_useLevel[0], v.m_useLevel[1]);
    if (!m_str.empty())
    {
        m_str += ";";   // ORIG 分隔串 @0x8ce2df7 为 ';'
    }
    m_str += num;
