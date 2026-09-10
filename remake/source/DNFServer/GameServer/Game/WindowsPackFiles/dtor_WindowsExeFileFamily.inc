WindowsExeFileFamily::~WindowsExeFileFamily()                // 0x08574500
{
    // [b28d-64] 同 ~WindowsDataFileFamily：ORIG 0x0857450e iterator C1Ev +
    // begin 临时槽（-0x1c）⇒ 先声明后赋值。
    std::vector<WindowsExeFile*>::iterator it;
    it = m_exeFiles.begin();
    for (; it != m_exeFiles.end(); ++it)
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
    }
    m_exeFiles.clear();
