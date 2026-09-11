// ===========================================================================
// WindowsDataFileFamily —— 0x08574f16..
// ===========================================================================
WindowsDataFileFamily::WindowsDataFileFamily()
    : m_files()
{
    memset(m_name, 0, sizeof(m_name));   // ORIG memset(m_name) 先于四标量清零(全于 body)
    m_scanOffset = 0;
    m_maxSize = 0;
    m_randMin = 0;
    m_randMax = 0;
}