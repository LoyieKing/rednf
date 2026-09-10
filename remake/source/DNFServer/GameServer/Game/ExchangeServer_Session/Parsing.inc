bool CSession::Parsing(int recvLen)
{
    int parsingLength;
    unsigned int packetSize;
    int moved;
    long long tick;
    char header[HEADER_SIZE];

    parsingLength = (int)m_recvOffset + recvLen;

    tick = get_ms_tick();
    m_lastRecvTick = (unsigned int)tick;
    m_recvGapTick = (unsigned int)(tick >> 32);

    if (parsingLength <= 12)
    {
        return 1;
    }

    packetSize = 0;
    do
    {
        if (m_recvOffset != 0)
        {
            m_recvBuf -= m_recvOffset;
        }

        memcpy(header, (const void*)m_recvBuf, HEADER_SIZE);
        packetSize = *(unsigned int*)(header + 3);
        if (packetSize <= 0xc || packetSize > 0x17318)
        {
            LogManager::logFormat(1, "ExchangeServerNetwork.h",
                __PRETTY_FUNCTION__, 0x18d,
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large",
                recvLen, (int)packetSize);
            return 0;
        }

        if (parsingLength < (int)packetSize)
        {
            break;
        }

        if (ES() != 0)
        {
            moved = ES()->DispatchPacket(this, (char*)m_recvBuf, (int)packetSize);
            if (moved != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x197, 0)(
                    "DispatchPacket() error line number(%d)", 407);
            }
        }

        parsingLength -= (int)packetSize;
        m_recvBuf += (int)packetSize;
        m_recvOffset = 0;

        if (parsingLength == 0)
        {
            m_recvBuf = m_recv;
            break;
        }
    } while (parsingLength > 5);

    if (parsingLength <= 0)
    {
        return 1;
    }
    if (parsingLength > 0x17318)
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            __PRETTY_FUNCTION__, 0x1b7,
            "[PARSING LENGTH EXCEPTION] parsinglength > MAX_SESSION_BUFFER_SIZE , memmove : parsinglength = %d",
            parsingLength);
        return 0;
    }

    try
    {
        memmove(m_recv, m_recvBuf, parsingLength);
        m_recvOffset = parsingLength;
        m_recvBuf = m_recv + parsingLength;
    }
    catch (...)
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            __PRETTY_FUNCTION__, 0x1c1,
            "[PARSING EXCEPTION] memmove : parsinglength = %d", parsingLength);
        return 0;
    }

    return 1;
