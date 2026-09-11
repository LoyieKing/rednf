// ORIG W 0x084ed2fc (size 0x34)：m_loadedLetterCount-1；负数归零。
void WongWork::CMailBox::DecLoadedLetterCount()
{
    --m_loadedLetterCount;
    if (m_loadedLetterCount < 0)
        m_loadedLetterCount = 0;
}