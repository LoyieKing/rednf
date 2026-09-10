WindowsCodeProtectorServer::FindDataFamilyFile(const char* name) // 0x0857562e
{
    // [b28d-58] P50 复活：ORIG 0x0857562e 先默认构造 iterator（C1Ev @0x08576da0）
    // + begin() 临时槽（-0x1c）再赋值 `it = begin()`；初始化式形被 4.4 折叠。
    std::vector<WindowsDataFileFamily*>::iterator it;
    it = m_dataFamilies.begin();
    for (; it != m_dataFamilies.end(); ++it)
    {
        WindowsDataFileFamily* f = *it;
        if (strcmp(f->m_name, name) == 0)
        {
            return f;
        }
    }
    return 0;
