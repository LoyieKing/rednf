bool PacketBuf::compress_packet()
{
    static Mutex compressed_buffer_mutex;
    static char compressed_buffer[MAX_PACKET_SIZE];

    int ret;
    unsigned long compressedSize;
    int uncompressedSize = m_size - PKT_BODY_OFF;
    if (uncompressedSize <= 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x36e,
                              "error; compress failed:unexpected message size");
        return false;
    }

    compressedSize = uncompressedSize + 13;
    compressed_buffer_mutex.lock();
    ret = compress((unsigned char*)compressed_buffer, &compressedSize,
                   (unsigned char*)(m_ptr + PKT_BODY_OFF), uncompressedSize);

    if (ret != 0 || compressedSize > MAX_PACKET_SIZE - PKT_BODY_OFF)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x380,
                              "error; compress failed:%d", ret);
        compressed_buffer_mutex.unlock();
        return false;
    }

    memcpy(m_ptr + PKT_BODY_OFF, compressed_buffer, compressedSize);
    compressed_buffer_mutex.unlock();
    m_size = (int)(compressedSize + PKT_BODY_OFF);
    *(int*)(m_ptr + PKT_LEN_OFF) = m_size;
    return true;
}