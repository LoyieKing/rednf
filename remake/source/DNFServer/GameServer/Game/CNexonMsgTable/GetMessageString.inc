const char* CNexonMsgTable::GetMessageString(int idx)   // ORIG T 0x0858010c
{
    std::map<int, std::string>::iterator it = m_messages.find(idx);
    if (it != m_messages.end())
        return it->second.c_str();
    return "";                            // ORIG "" @0x08ca9390
