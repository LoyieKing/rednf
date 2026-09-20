wchar_t* WideStringData::getBuffer()
{
    return reinterpret_cast<wchar_t*>(m_buf);
}