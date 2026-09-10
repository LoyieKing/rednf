bool DbmwClient::set_query(const char* fmt, ...)
{
    if (fmt == 0)
        return false;
    memset(m_query, 0, 0x1001);
    va_list args;
    va_start(args, fmt);
    vsprintf(m_query, fmt, args);
    va_end(args);
    m_queryLen = strlen(m_query);
    // ORIG 0x08120572/0x08120582：`mov 0x1038(%eax),%eax; cmp $0xfff,%eax; jg`
    // （有符号 jg）⇒ m_queryLen 声明 long（signed）后无需 cast 即出 jg。
    if (m_queryLen > 0xfff || m_queryLen <= 7)
    {
        // ORIG 0x0812059c：格式串含两个 %d 且第二实参为立即数 0x1000
        // （objdump .rodata 实证）；原实现漏了 "MAX_QUERY_SIZE-%d\n" 段。
        printf("QueryLength Size ERROR QUERY_SIZE-%d MAX_QUERY_SIZE-%d\n",
               (int)m_queryLen, 0x1000);
        return false;
    }
    // ORIG 0x081205c1：`movl $"select",(%esp); mov m_query,0x4(%esp)` ⇒ 参数序
    // 是 strncasecmp("select", m_query, 6)；且结果用 jne/两臂 movb 直写
    // （ORIG 0x081205d1/0x081205d9），非 sete 单条物化。
    if (strncasecmp("select", m_query, 6) == 0)
        m_isSelect = 1;
    else
        m_isSelect = 0;
    return true;
