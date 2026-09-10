void WongWork::CMailBox::SetLoadedLetterCount(int count)
{
	// ORIG 0x085789c：m_loadedLetterCount(+0x80) = count 直写。
	m_loadedLetterCount = count;
