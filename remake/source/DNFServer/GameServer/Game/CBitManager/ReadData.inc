void CBitManager::ReadData(unsigned char* data, int size)   // 0x0831c116
{
    m_data = data;
    m_size = size;
    m_buffer = static_cast<unsigned char*>(malloc(size));
    memset(m_buffer, 0, size);
