void WongWork::CMailBox::incMailLoadCount()
{
	// ORIG 0x085575dc：m_mailLoadCount(+0x0c) 自增，超过 0x7ffffffe 归零
	//（cmp $0x7ffffffe; jbe 无符号判定）。
	m_mailLoadCount = m_mailLoadCount + 1;
	if ((unsigned int)m_mailLoadCount > 0x7ffffffeU)
		m_mailLoadCount = 0;
