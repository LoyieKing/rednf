// ORIG 0x089fb78e：deque 默认构造 → clear()（带 unwind 清理路径）
NpcMovieData::NpcMovieData()
    : m_movies()
{
    clear();
}