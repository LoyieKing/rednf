void PacketBuf::encFinalize()
{
    unsigned int crc1 = Encryption::crc32N(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF);
    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + PKT_LEN_OFF) = (char)m_size;
        *(char*)(m_ptr + PKT_LEN_OFF + 1) = (char)((m_size & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_LEN_OFF + 2) = (char)((m_size & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_LEN_OFF + 3) = (char)((unsigned int)m_size >> 24);
        break;
    case 1:
        *(char*)(m_ptr + PKT_LEN_OFF) = (char)((unsigned int)m_size >> 24);
        *(char*)(m_ptr + PKT_LEN_OFF + 1) = (char)((m_size & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_LEN_OFF + 2) = (char)((m_size & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_LEN_OFF + 3) = (char)m_size;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x419,
                              "m_byte_encoding error %d", m_byte_encoding);
        return;
    }

    unsigned int crc2 = Encryption::crc32N(m_ptr + PKT_BODY_OFF, m_size - PKT_BODY_OFF);
    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + PKT_CRC1_OFF) = (char)crc1;
        *(char*)(m_ptr + PKT_CRC1_OFF + 1) = (char)((crc1 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC1_OFF + 2) = (char)((crc1 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC1_OFF + 3) = (char)(crc1 >> 24);
        *(char*)(m_ptr + PKT_CRC2_OFF) = (char)crc2;
        *(char*)(m_ptr + PKT_CRC2_OFF + 1) = (char)((crc2 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC2_OFF + 2) = (char)((crc2 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC2_OFF + 3) = (char)(crc2 >> 24);
        break;
    case 1:
        *(char*)(m_ptr + PKT_CRC1_OFF) = (char)(crc1 >> 24);
        *(char*)(m_ptr + PKT_CRC1_OFF + 1) = (char)((crc1 & 0xff0000) >> 16);
        *(char*)(m_ptr + PKT_CRC1_OFF + 2) = (char)((crc1 & 0xff00) >> 8);
        *(char*)(m_ptr + PKT_CRC1_OFF + 3) = (char)crc1;
        *(char*)(m_ptr + PKT_CRC2_OFF) = 0;
        *(char*)(m_ptr + PKT_CRC2_OFF + 1) = 0;
        *(char*)(m_ptr + PKT_CRC2_OFF + 2) = (char)((crc2 & 0xff0000) >> 8);
        *(char*)(m_ptr + PKT_CRC2_OFF + 3) = 0;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x439,
                              "m_byte_encoding error %d", m_byte_encoding);
        return;
    }
}