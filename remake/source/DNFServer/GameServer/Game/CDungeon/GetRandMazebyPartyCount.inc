int CDungeon::GetRandMazebyPartyCount(int partyCount) const
{
    // [b28d-solo-CDungeon2 判词 · NEAR（DIFF→NEAR）] ORIG 0x0834d06c / our 131
    //（ORIG 133，gap 2）；p167 18=18 CLEAR。历史修法：两处 `(int)candidates[x]`
    // 改 `*(int*)&candidates[x]` 视图读（P266）；删 local_1c 具名局部；r 声明 int。
    // 本轮修法：删具名 `unsigned int sz` 改匿名 `rand() % m_mazeList.size()` /
    // `rand() % candidates.size()`（P0：ORIG 的 -0x2c 是 `divl` 的内存操作数临时槽，
    // 非具名局部）⇒ 4.4 临时槽落 -0x2c、帧 0x2ac→0x40 与 ORIG 全等。
    // 残余（N 封顶）：①ORIG 余数 `mov %edx,%ecx; mov %ecx,%eax` 双中转 vs 4.4
    // `mov %edx,%eax`（N18a 单段 regmove 不可造，×2 处）；②块内变量槽方向差
    //（elem -0x24 vs -0xc、i -0xc vs -0x10、r -0x10 vs -0x14、vector -0x20 vs -0x24；
    // 4.1 块内变量占浅槽 / 4.4 占深槽，N82/N93 族；声明序三变体 + elem 循环内声明
    // 实测均不可翻）。调用点差集/槽位集逐条核对成立。
	// ORIG 0x0834d06c：唯一迷宫直返；否则按 [m_partyCountMin, m_partyCountMax]
	// 收集候选；候选唯一/随机取，空则全表随机。
	if (m_mazeList.size() == 1)
		return (int)&m_mazeList[0];

	std::vector<MazeScript*> candidates;
	int r;
	unsigned int i;
	MazeScript* elem;

	i = 0;
	while (m_mazeList.size() > i)
	{
		elem = const_cast<MazeScript*>(&m_mazeList[i]);
		if (elem->m_partyCountMin <= partyCount &&
			partyCount <= elem->m_partyCountMax)
		{
			candidates.push_back(static_cast<MazeScript*&&>(elem));
		}
		i = i + 1;
	}

	r = 0;
	if (candidates.empty())
	{
		r = rand() % m_mazeList.size();
		return (int)&m_mazeList[r];
	}
	if (candidates.size() == 1)
		return *(int*)&candidates[0];
	r = rand() % candidates.size();
	return *(int*)&candidates[r];
