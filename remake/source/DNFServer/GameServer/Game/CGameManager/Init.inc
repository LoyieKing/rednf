void WongWork::CMailBox::Init()
{
	// ORIG 0x08551d98：m_bInit==0 早退；否则遍历 m_mailMap 重置并释放邮件、
	// 清空各容器与计数。迭代器形态：it 默认构造 + end 直构缓存 + begin 经临时
	// 拷贝（P28）；循环为 while（前置 jmp）。
	if (!m_bInit)
		return;
	CMail* mail;   // ORIG 槽 -0xc（先于迭代器声明；P28 声明序即栈槽序）
	std::map<unsigned int, CMail*>::iterator it;
	std::map<unsigned int, CMail*>::iterator endIt = m_mailMap.end();
	it = m_mailMap.begin();
	while (it != endIt)
	{
		mail = it->second;
		mail->ResetPackage();   // ORIG call CMail::ResetPackage（0x085519fe）
		m_mailPool.Free(mail);
		++it;
	}
	m_mailMap.clear();
	m_bInit = false;
	m_lastLoadIdx = 0;
	m_notLoadedMailCount = 0;
	if (!m_streamList.empty())
	{
		std::list<Stream*>::iterator sit = m_streamList.begin();
		std::list<Stream*>::iterator send = m_streamList.end();
		while (sit != send)
		{
			// ORIG 无 Stream* 局部、无判空：*it 直取即 Free（mov (%eax),%edx）。
			GlobalData::s_stream_pool->Free(*sit);
			++sit;
		}
	}
	m_streamList.clear();
	m_letterKeepCount.clear();
	m_lastLoadLetterIdx = 0;
	m_loadedLetterCount = 0;
	m_loadLackList.clear();
		m_letterInfo.clear();
}