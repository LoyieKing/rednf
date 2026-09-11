// ORIG getParam 0x084ab64e：按值接收参数副本并切分。
void CStartEvent::getParam(std::string param)
{
    int pos = 0;
    std::string params[16];
    while ((pos = static_cast<int>(param.find(' ', 0))) != -1)
    {
        params[m_eventCount] = param.substr(0, pos);
        param = param.substr(pos + 1, std::string::npos);
        m_eventCount = m_eventCount + 1;
    }
    params[m_eventCount] = param;
    m_eventId = atoi(params[0].c_str());
    m_param1 = static_cast<unsigned short>(atoi(params[1].c_str()));
    m_param2 = static_cast<unsigned short>(atoi(params[2].c_str()));
}