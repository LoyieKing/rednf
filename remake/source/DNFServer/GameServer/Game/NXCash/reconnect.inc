bool NCashImpl::reconnect()
{
    if (!m_connected)
    {
        m_socket.close();
        m_socket.open();
        m_connected = m_socket.connect(m_serverAdrs,
                                       static_cast<unsigned short>(m_port));
    }
    return m_connected;
