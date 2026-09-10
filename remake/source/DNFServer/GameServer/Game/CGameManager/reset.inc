void WongWork::CMailBox::reset()
{
	// ORIG 0x08551f4a：仅清 m_mailLoadCount(+0x0c) 一个 dword。
	m_mailLoadCount = 0;
