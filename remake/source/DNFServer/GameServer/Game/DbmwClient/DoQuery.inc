bool DbmwClient::DoQuery()
{
    static const char UPDATE_QUERY_GO = 1;  // ORIG _ZZN10DbmwClient7DoQueryEvE15UPDATE_QUERY_GO
    // ORIG @0x08120cd0。
    m_affectedRow = 0;
    // 技巧 5（signedness 装载序定律）：ORIG 是 `mov %eax,-0x10(%ebp)` 之后
    // **两条内存-立即数直比** `cmpl $0xfff,-0x10(%ebp); jg` +
    // `cmpl $0x7,-0x10(%ebp); jg`。用 `(long)qlen` 会迫使 4.4 先把槽值装进
    // %eax 再 `cmp`（实测多 1 条 mov、2 条 cmp 形态失配）。
    // ⇒ 局部必须是 **int**（ORIG 槽宽 4B、比较用有符号 jg），且比较写成
    //   `> 0xfff` 而非 `>= 0x1000`，让立即数与 ORIG 完全一致。
    int qlen = (int)strlen(m_query);
    if (qlen > 0xfff || qlen <= 7)
        return false;
    // ORIG 0x08120d0a：`mov 0x1038(%eax),%eax; cmp -0x10(%ebp),%eax`（成员在
    // %eax、栈槽作内存操作数直比）⇒ 字段与 qlen 同型（都 signed 32 位）时
    // 4.4 才出该形。
    if (m_queryLen != qlen)
        return false;

    if (!SendHeader(UPDATE_QUERY_GO, (int)m_queryLen))
    {
        Close();
        return false;
    }
    ssize_t sent = ::send(m_sock, m_query, m_queryLen, 0);
    if (sent < 1)
    {
        Close();
        return false;
    }
    return true;
