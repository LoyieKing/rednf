void CDungeon::GetRandMaze(int& out) const
{
	out = rand() % m_mazeList.size();
	(void)m_mazeList[out];
