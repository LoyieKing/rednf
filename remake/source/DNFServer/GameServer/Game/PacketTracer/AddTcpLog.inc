void CPacketTracer::AddTcpLog(int flag, int value)
{
    int idx = m_tcpCount % 10;

    m_logs[0][idx].m_flag = static_cast<char>(flag);
    m_logs[0][idx].m_value = value;
    ++m_tcpCount;
