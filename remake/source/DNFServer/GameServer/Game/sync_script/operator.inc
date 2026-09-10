void CStringMaker::operator()(int v)                  // 0x08375f6c "%d"
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    sprintf(num, "%d", v);
    if (!m_str.empty())
    {
        m_str += ",";
    }
    m_str += num;
