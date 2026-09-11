void WongWork::CMailBox::SetNotLoadedMailCount(int count)
{
	// ORIG 0x08557914：m_notLoadedMailCount(+0x08) = count 直写。
	m_notLoadedMailCount = count;
}