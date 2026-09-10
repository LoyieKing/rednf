void WongWork::CMailBox::SetLastLoadLetterIdx(unsigned int idx)
{
	// ORIG 0x08557858：m_lastLoadLetterIdx(+0x7c) = idx 直写。
	m_lastLoadLetterIdx = idx;
