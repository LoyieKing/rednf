// _ZN10CSimpleLog13CreateLogFileEPKc @ 0x0854fbba
void CSimpleLog::CreateLogFile(char const* path)
{
    DateTime dt;
    strncpy(m_path, path, 0xfe);
    CreateLogFile(dt);
}