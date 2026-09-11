// ORIG 0x089fb7ee：返回 std::string（sret 模式）。early-return 结构（P16）：
// 队列空 → 直接返回；游标 < size 时取 deque[游标] 拷入返回值，
// 游标 = (游标+1) % size（无符号 divl 实证）。
std::string NpcMovieData::getNextMoviePath()
