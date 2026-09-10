void CDataManager::set_AuctionRegFreeCeraItem(unsigned int idx)
{
    // ORIG 0x08513fbc W：m_choiceSet(+0xb4ac).insert(idx)
    m_choiceSet.insert(idx);
