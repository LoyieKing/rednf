// ============================================================================
// NpcMovieData.cpp — NpcMovieData 强符号（T）实现
//
// 来源（df_game_r，elf32-i386）：
//   NpcMovieData::NpcMovieData()        T 0x089fb78e
//   NpcMovieData::clear()               T 0x089fb7d0
//   NpcMovieData::getNextMoviePath()    T 0x089fb7ee
//     （docs/class_func_reports/NpcMovieData/）
// 析构为 ORIG 弱符号（W 0x08581d96，仅 deque dtor），头内联空析构即可。
// 布局：std::deque<std::string>@+0x00 + int@+0x28（sizeof 0x2c）。
// ============================================================================
#include "NpcMovieData.h"

// ORIG 0x089fb78e：deque 默认构造 → clear()（带 unwind 清理路径）
NpcMovieData::NpcMovieData()
    : m_movies()
{
    clear();
}

// ORIG 0x089fb7d0：deque::clear + 游标置 0
void NpcMovieData::clear()
{
    m_movies.clear();
    m_movieIdx = 0;
}

// ORIG 0x089fb7ee：返回 std::string（sret 模式）。early-return 结构（P16）：
// 队列空 → 直接返回；游标 < size 时取 deque[游标] 拷入返回值，
// 游标 = (游标+1) % size（无符号 divl 实证）。
std::string NpcMovieData::getNextMoviePath()
{
    std::string result;
    if (m_movies.empty())
    {
        return result;
    }
    if ((int)m_movies.size() > m_movieIdx)
    {
        result = m_movies[m_movieIdx];
        m_movieIdx = (unsigned)(m_movieIdx + 1) % (unsigned)m_movies.size();
    }
    return result;
}

// ---- ORIG W 弱符号发射段（D1/D2 @0x08581d96，头内联空析构）----
// 本 TU 既有函数不 odr-use 析构（无 NpcMovieData 实例化），头内联空析构的
// 弱符号不会发射；此处真实构造/析构强制发射，不引入导出符号。
namespace
{
void emitNpcMovieDataDtor()
{
    NpcMovieData m;
    (void)m;
}
}
