void CPacketVerifier::Init() throw()   // P216：头（Main 代落 CUser.h:938）同步；
                                       // C1 对 Init call 不再布 EH cleanup
{
    m_lastClass = 0;
    m_lastId = 0;
    m_lastChecksum = 0;
    m_sameCount = 0;
    m_sameTime = 0;
    m_sequence = 0;
    m_auctionCount = 0;
    m_auctionTime = 0;
    m_checker.reset();
