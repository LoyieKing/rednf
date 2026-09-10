void CommonTime::SetCreateTime(char* src)
{
    // 格式 "xxYYYY-MM-DD HH:MM"：跳过 2 字符前缀，按 2 字符 token + 1
    // 分隔符解析（ORIG 语义，只解析 5 段：年月日时分）
    char buf[3] = {0};
    src += 2;
    strncpy(buf, src, 2);
    m_year = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_month = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_day = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_hour = (char)atoi(buf);
    src += 3;
    strncpy(buf, src, 2);
    m_min = (char)atoi(buf);
