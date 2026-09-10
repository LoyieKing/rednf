bool WindowsCodeProtectorServer::LoadWindowsFile(const char* path,
                                                 const char* name) // 0x08575712
{
    // [b28d-solo-WindowsPackFiles] ORIG 0x08575729 `test %eax,%eax; je +0x120`：
    // **exe 域物理在前（fall-through）、data 域在后（je 目标）** ⇒ 源码为
    // `if (strstr(...) != 0) { exe } else { data }`；且**全函数只有一枚
    // `mov $0x0,%eax`（+0x20c）**，所有失败路径 `je +0x20c` 直达共享尾 ⇒ 源码
    // 必须写成「单 return false 出口」（if/else + else-if 链），不能每支 return false
    // （4.4 会就地复制 `mov $0; jmp` 块，旧版 173 vs 163 的 +10 即此）。
    if (strstr(path, ".exe.bak") != 0)                       // exe 文件域
    {
        if (m_exeFamily == 0)                                // 首个 .exe.bak
        {
            m_exeFamily = new WindowsExeFileFamily;
            if (m_exeFamily->LoadWindowsExeFile(path, name))
            {
                m_exeFamily->SetCheckingOption(
                    static_cast<unsigned long>(m_exeMin),
                    static_cast<unsigned long>(m_exeMax));
                return true;
            }
        }
        else
        {
            // ORIG 两守卫各自 `test %al,%al; je <共享 mov $0>`（无 && 物化岛）⇒
            // P133/P225：拆两条嵌套 if，`a && b` 会被 4.4 折成 mov$1/mov$0 物化。
            if (FindExeFamilyFile(name))
            {
                if (m_exeFamily->LoadWindowsExeFile(path, name))
                {
                    m_exeFamily->SetCheckingOption(
                        static_cast<unsigned long>(m_exeMin),
                        static_cast<unsigned long>(m_exeMax));
                    return true;
                }
            }
        }
    }
    else
    {
        // data 文件域（ORIG +0x120 起）
        WindowsDataFileFamily* family = FindDataFamilyFile(name);
        if (family == 0)
        {
            family = new WindowsDataFileFamily;
            if (family->LoadWindowsDataFile(path, name))
            {
                family->SetCheckingOption(
                    static_cast<unsigned long>(m_dataMin),
                    static_cast<unsigned long>(m_dataMax));
                m_dataFamilies.push_back(family);
                return true;
            }
            delete family;
        }
        else if (family->LoadWindowsDataFile(path, name))
        {
            return true;
        }
    }
    return false;
