void CStringMaker::operator()(ItemAuraInfo v)         // 0x08627066 "%d,%d,%d,%d"
{
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    // ORIG 参数栈：v+0 int / (uchar)v+4 / float v+8（vararg 提升 double）/
    // v+c int，fmt "%d,%d,%d,%d"。
    sprintf(num, "%d,%d,%d,%d", v.m_field0, (int)v.m_field4, v.m_float8,
            v.m_fieldc);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
}