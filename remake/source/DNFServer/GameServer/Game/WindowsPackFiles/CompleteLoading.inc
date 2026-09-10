bool WindowsCodeProtectorServer::CompleteLoading()           // 0x0857592c
{
    // ORIG 双份 `movb $1,0x1c` 常量岛（+0x23/+0x38）= 4.1 对两条返回路径各自
    // 展开赋值；内层为正向 `if (call())`（+0x1c test%al; je 无 xor）。
    if (m_exeFamily)
    {
        if (m_exeFamily->CompleteLoadingWindowsExeFiles())
        {
            m_loaded = 1;
            return true;
        }
        return false;
    }
    m_loaded = 1;
    return true;
