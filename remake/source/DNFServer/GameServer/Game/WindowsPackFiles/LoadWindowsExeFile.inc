bool WindowsExeFile::LoadWindowsExeFile(const char* path)    // 0x085740de
{
    if (!LoadClientFile(&m_size, reinterpret_cast<void**>(&m_image), path))
    {
        return false;
    }
    if (!ReadPeHeader())
    {
        delete m_image;      // ORIG _ZdlPv（同 ~WindowsExeFile 保留原版形态）
        m_image = 0;
        m_size = 0;
        return false;
    }
    Sort();                                                  // common 尚空——按 ORIG 形态保留调用
    return true;
