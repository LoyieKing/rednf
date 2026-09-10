void WongWork::CMailBox::DecLoadedLetterCount()
{
    --m_loadedLetterCount;
    if (m_loadedLetterCount < 0)
        m_loadedLetterCount = 0;
