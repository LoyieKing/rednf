void PacketBuf::clear()
{
    m_use_size = 0;
    m_size = 0;
    m_finalized = false;
    m_ptr = m_buf;
    setLastError(PACKETBUF_ERROR_NONE);
    m_encrypt_required = true;
    clear_ptr();
}