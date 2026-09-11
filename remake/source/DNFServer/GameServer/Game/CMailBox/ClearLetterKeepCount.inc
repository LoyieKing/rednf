// ORIG 0x08557866 (W)：clear keep map 并清零 lastLoadLetterIdx
void CMailBox::ClearLetterKeepCount()
{
    m_letterKeepCount.clear();
    m_lastLoadLetterIdx = 0;
}