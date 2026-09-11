void WindowsCodeProtectorServer::Destroy()                   // 0x08575500
{
    if (m_exeFamily != 0)
    {
        delete m_exeFamily;
        m_exeFamily = 0;
    }
    // ORIG 0x08575500：独立 iterator::C1Ev（-0x10）+ begin() 临时（-0x1c）后
    // 拷贝赋值 ⇒ P50 复活「先声明后赋值」形；while 条件（end/ne）在循环尾。
    std::vector<WindowsDataFileFamily*>::iterator it;
    it = m_dataFamilies.begin();
    while (it != m_dataFamilies.end())
    {
        if (*it != 0)
        {
            delete *it;
            *it = 0;
        }
        ++it;
    }
    m_dataFamilies.clear();
}