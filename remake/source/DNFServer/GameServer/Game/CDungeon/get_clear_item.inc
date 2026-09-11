int CDungeon::get_clear_item() const
{
	// ORIG 0x0834c8c0：ret 先置栈、迭代器默认构造、lower_bound 到临时再赋值
	int ret = 0;
	int key;
	std::map<int, int>::const_iterator it;
	key = get_rand_int(100) + 1;
	it = m_clearItemMap.lower_bound(key);
	if (it == m_clearItemMap.end())
		ret = -2;
	else
		ret = it->second;
	return ret;
}