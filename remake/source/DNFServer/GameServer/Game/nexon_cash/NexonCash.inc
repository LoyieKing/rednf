NexonCash::NexonCash(const char* serverAdrs, short port, char paramB,
                     short fieldC, short fieldD)
{
    m_impl = new NCashImpl(serverAdrs, port,
                           static_cast<unsigned char>(paramB),
                           static_cast<unsigned short>(fieldC), fieldD);
