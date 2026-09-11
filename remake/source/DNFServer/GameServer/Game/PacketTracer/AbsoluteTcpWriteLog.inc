void CPacketTracer::AbsoluteTcpWriteLog()
{
    char buf[0x800] = { '\0' };
    for (int i = 0; i <= 9; ++i)
    {
        std::sprintf(buf, "%s,%d/%d", buf,
                     static_cast<int>(static_cast<unsigned char>(m_logs[0][i].m_flag)),
                     m_logs[0][i].m_value);
    }
    --m_tcpCount;
    register unsigned int idx = m_tcpCount % 10;
    cMyTrace trace(__FUNCTION__, 0x41, 0);
    trace("[TRACE_TCP_PACKET] (idx:%d)%s", idx, buf);
}