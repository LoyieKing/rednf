bool CSimpleLog::CloseFile()
{
    if (m_fp != 0) {
        fclose(m_fp);
        m_fp = 0;
    }
    return 1;
