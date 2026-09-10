void CSimpleLog::CreateLogFile(char const* path)
{
    DateTime dt;
    strncpy(m_path, path, 0xfe);
    CreateLogFile(dt);
