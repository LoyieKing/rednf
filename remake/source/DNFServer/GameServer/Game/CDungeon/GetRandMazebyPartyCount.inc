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
    // [L6S2 复验] 真实旗标 4.4.7 穷举 24 种声明排列 ×｛块内, 函数头｝共 48 形
    //（探针 /tmp/L6S2_probe/pr_rand2.cpp，解析 analyze_rand.py）：vec 与 i/r 可对上
    // ORIG（v30-v35/v38-v47：vec -0x24、i -0x10、r -0xc），但 elem 恒落 -0x18/-0x24
    //（ORIG -0xc）且 push_back 实参临时恒落 -0x14（ORIG -0x18）⇒ 4.4 把指针类对象
    // 与整型标量分区分配、ORIG 单列混排（4.1 声明序连续槽 -0x24..-0xc 全对齐），
    // 声明序不可达 ⇒ N82/N98 槽图封顶，勿再试。
    // 另：p167 18=18 差异全为 C1/C2、D1/D2 同址别名（P93，nm 实证无真缺口）。
	// ORIG 0x0834d06c：唯一迷宫直返；否则按 [m_partyCountMin, m_partyCountMax]
	// 收集候选；候选唯一/随机取，空则全表随机。
	if (m_mazeList.size() == 1)
		return (int)&m_mazeList[0];

	std::vector<MazeScript*> candidates;
	int r;
	unsigned int i;
	// [PoolE 2026-09-11 实验 A] ORIG cur 槽 -0xc（浅区）；4.4 把 `MazeScript*` 归
	// 指针分区恒落 -0x18/-0x24（L6S2 48 形穷举）⇒ 试以整型承载指针地址（32 位等价，
	// 转换零指令），期望 elemAddr 落整型分区 -0xc 且其余局部整体下移 4 与 ORIG 对齐。
	unsigned int elemAddr;

	i = 0;
	while (m_mazeList.size() > i)
	{
		elemAddr = (unsigned int)&m_mazeList[i];
		if (((MazeScript*)elemAddr)->m_partyCountMin <= partyCount &&
			partyCount <= ((MazeScript*)elemAddr)->m_partyCountMax)
		{
			candidates.push_back((MazeScript*)elemAddr);
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
}