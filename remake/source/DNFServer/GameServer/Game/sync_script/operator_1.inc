void CStringMaker::operator()(float v)                // 0x08626cd0 "%f"
{
    // b28d-35：ORIG 缓冲 0xff 字节为 4.1 内联三段清零（对齐+字+双字 rep stos）；
    //   `memset` 调用形在 4.4 恒 call。按 P214③/P253 改 `={0}` 复现内联 rep stos。
    char num[0xff] = {0};
    sprintf(num, "%f", v);
    if (!m_str.empty())
    {
        m_str += ",";
    }
    m_str += num;
}