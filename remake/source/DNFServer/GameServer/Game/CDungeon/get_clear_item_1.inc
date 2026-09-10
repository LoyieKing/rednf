int CDungeon::get_clear_item(int level) const
{
	// ORIG 0x0834c95c：同 get_clear_item()，key = level + 1
	int ret = 0;
	int key;
	std::map<int, int>::const_iterator it;
	key = level + 1;
	it = m_clearItemMap.lower_bound(key);
	if (it == m_clearItemMap.end())
		ret = -2;
	else
		ret = it->second;
	return ret;
