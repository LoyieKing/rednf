// ORIG W 0x080cbc6c：movl $0,0x0 + movb $0,0x4/0x5/0x6 —— 清 eventId/m_pad4/isWin/needSave。
// ORIG C1 0x08697128：movl $0,+0 / movb $0,+4/+5/+6（m_eventId/m_pad4/m_isWin/m_needSave）。
BingoData::BingoData()
{
    m_eventId = 0;
    m_pad4 = 0;
    m_isWin = false;
    m_needSave = false;
}