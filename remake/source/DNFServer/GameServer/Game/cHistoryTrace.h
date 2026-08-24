// ============================================================================
// cHistoryTrace.h — 历史日志 trace 仿函数（全局类，无命名空间）
//
// ORIG 证据（df_game_r, elf32-i386）：
//   - 平凡类：ORIG 无 ctor/dtor 符号（handleLeaveUser 中栈上对象无构造/析构
//     调用），sizeof == 1（空类）。
//   - operator()(char const*, ...) 0x0854f990：
//     Guard<Mutex>(CLog::instance() + 0xa8) 加锁后
//     CLog::instance()->logHistory(fmt, va_list)，析构解锁。
//     （+0xa8 = CLog::m_mutexes[7]，Mutex 0x18 字节。）
//
// 权威声明点；实现位于 HistoryLog.cpp。原 CBossTower.cpp 局部声明已由本头替换。
// ============================================================================
#ifndef DNF_GAME_CHISTORYTRACE_H_
#define DNF_GAME_CHISTORYTRACE_H_

class cHistoryTrace
{
public:
    void operator()(char const* fmt, ...);   // ORIG 0x0854f990
};

#endif  // DNF_GAME_CHISTORYTRACE_H_
