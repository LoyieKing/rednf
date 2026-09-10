void CExchangeServer::Reset()
{
    m_flag38 = false;
    m_flag39 = false;
    // N92 翻案（b28 L11）：中转仅见于 long→unsigned int 字段；字段改 time_t 同型(long)
    // 后直存形 `mov 0x8(%ebp),%edx; mov %eax,0x3c(%edx)` 与 ORIG 全等（探针 l11_n92）。
    m_resetTime3c = time(NULL);
