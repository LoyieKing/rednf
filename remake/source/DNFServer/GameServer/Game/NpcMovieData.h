#ifndef GAME_NPCMOVIEDATA_H_
#define GAME_NPCMOVIEDATA_H_

#include <deque>
#include <string>

// ---- NpcMovieData（ORIG C1/clear/getNextMoviePath T 0x089fb78e/0x089fb7d0/
//      0x089fb7ee → cpp 定义；D1 W 0x08581d96 → 头内联）----
// 布局（clear/getNextMoviePath 实证）：std::deque<std::string>@+0x00 +
// int@+0x28（播放游标），sizeof 0x2c。
// C1：deque 默认构造 → clear()；clear：deque::clear + 游标置 0；
// getNextMoviePath：队列非空且游标 < size 时取 deque[游标] 并
// (游标+1) % size（无符号模，ORIG divl 实证），返回 std::string。
// 报告: docs/class_func_reports/NpcMovieData
class NpcMovieData
{
public:
    // ORIG 0x89fb78e _ZN12NpcMovieDataC1Ev (size 0x00000042)  报告: docs/class_func_reports/NpcMovieData
    NpcMovieData();
    // ORIG 0x8581d96 _ZN12NpcMovieDataD1Ev (size 0x00000013)  报告: docs/class_func_reports/NpcMovieData
    ~NpcMovieData() {}
    // ORIG 0x89fb7ee _ZN12NpcMovieData16getNextMoviePathEv (size 0x000000c7)  报告: NpcMovieData/getNextMoviePath.md
    std::string getNextMoviePath();
    // ORIG 0x89fb7d0 _ZN12NpcMovieData5clearEv (size 0x0000001d)  报告: NpcMovieData/clear.md
    void clear();

    std::deque<std::string> m_movies;   // +0x00（movie path 队列）
    int m_movieIdx;                     // +0x28（播放游标）
};   // sizeof 0x2c

#endif  // GAME_NPCMOVIEDATA_H_
