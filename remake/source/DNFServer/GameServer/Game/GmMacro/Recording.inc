void CMacro::Recording(int type, int sub, char* data, int len)
{
    stMacroData rec;
    rec.m_cmd = static_cast<unsigned char>(type);
    rec.m_sub = static_cast<unsigned char>(sub);
    if (data != 0 && len != 0)
    {
        rec.m_data = new char[len];                     // ORIG operator new[](unsigned)
        memcpy(rec.m_data, data, len);
        rec.m_len = len;
    }
    m_data.push_back(rec);
