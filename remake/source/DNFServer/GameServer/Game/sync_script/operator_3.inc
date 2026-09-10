void CStringMaker::operator()(EquipmentSkillDataUp v) // 0x08626e6e 七个 %d
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    sprintf(num, "%d,%d,%d,%d,%d,%d,%d", v.m_job, v.m_v4, v.m_v8,
            v.m_percent, v.m_v10, v.m_key, v.m_module);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
