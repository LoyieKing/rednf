void WongWork::CMailBox::SetPackageLoadLack(const unsigned int* arr, int count,
                                            std::set<unsigned int>& oldLack)
{
	// ORIG 0x08557676：先把旧列表（m_loadLackList，+0x84）全部并入 oldLack，
	// 清空列表；随后把新数组逐项 push_back，并把在 oldLack 中出现者从中移除。
	// 迭代器形态：vector iterator 直构 begin，end() 在循环条件内每轮调用（非缓存）。
	std::vector<unsigned int>::iterator vit = m_loadLackList.begin();
	while (vit != m_loadLackList.end())
	{
		oldLack.insert(*vit);
		++vit;
	}
	m_loadLackList.clear();
	for (int i = 0; i < count; ++i)
	{
		m_loadLackList.push_back(arr[i]);
		std::set<unsigned int>::iterator sit = oldLack.find(arr[i]);
		if (sit != oldLack.end())
			oldLack.erase(sit);
	}
}