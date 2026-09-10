void selectiveGrowCreatureInfo::clear()
{
    // ORIG 0x833df64：growLevel/AType/BTypeItem/AType/BTypeCreature 置 -1，growItemCount 置 1。
    m_usingExpItem = 0;
    m_growLevel = -1;
    m_growATypeItem = -1;
    m_growBTypeItem = -1;
    m_growATypeCreature = -1;
    m_growBTypeCreature = -1;
    m_growItemCount = 1;
