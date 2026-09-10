char* MySQL::blob_to_str(int idx, void* data, int len)
{
    if (idx < 0 || idx > 9 || data == 0 || len > 0x5fff)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x267, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x267);
    }
    m_blob[idx][0] = 0;
    if (len > 0)
    {
        char* p = m_blob[idx];
        p = p + mysql_real_escape_string(m_dbHandle, p, (const char*)data, len);
        *p++ = 0;
    }
    return m_blob[idx];
