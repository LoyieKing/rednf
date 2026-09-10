void CQuestShop::_reset()
{
    m_buyingStatus.clear();          // 0x85efd83 map clear (+0x08)
    m_qp = 0;                        // movl $0,0x38(%eax)
    m_initCount = 0;                 // movl $0,0x3c(%eax)（ORIG 序：0x38→0x3c→0x40）
    m_initQp = 0;                    // movl $0,0x40(%eax)
    m_tagBuyingStatus.clear();       // +0x20 map clear
    m_piece = 0;                     // 0x44
