#ifndef GAME_OS_API_H_
#define GAME_OS_API_H_

// OS_API —— ORIG df_game_r 0x0858c53a..0x0858c81c 的 10 个全局符号。
// 均为自由函数（mangled _ZN6OS_API... 与 class static 成员一致，无 this）。
// 定义归属：
//   Exit/WriteLastBookmark/WriteLastSig×2/WriteLoginInfo → OS_API.cpp
//   GetDateTimeTick/snprintf                          → CSystemTime.cpp
//   Sleep                                             → LinkFinalDeps.cpp
//   WriteLog                                          → LinkMisc4_Data.cpp
//   uSleep                                            → W8LT_DBTask.cpp
// 返回类型按 docs/class_func_reports/OS_API 各报告（Ghidra 均 void；除
// GetDateTimeTick/snprintf 为 int）核实。

namespace OS_API
{
void Exit(int exitCode);                                   // ORIG 0x0858c53a
int  GetDateTimeTick();                                    // ORIG 0x0858c802
void Sleep(int ms);                                        // ORIG 0x0858c4e0
void WriteLastBookmark(char const* path, int value);       // ORIG 0x0858c6d4
void WriteLastSig(char const* dir, char const* name,       // ORIG 0x0858c54c
                  int k, int c, void const* p, int sz);
void WriteLastSig(char const* dir, char const* name,       // ORIG 0x0858c5f4
                  unsigned int u, int k, int c, void const* p,
                  int tq, int dq, int nq, int dbq, int logdbq, int extra);
void WriteLog(char const* fileName, char const* fmt, ...); // ORIG 0x0858c78a
void WriteLoginInfo(char const* path, char const* fmt, ...); // ORIG 0x0858c71e
int  snprintf(char* buf, unsigned int n, char const* fmt, ...); // ORIG 0x0858c81c
void uSleep(unsigned int usec);                            // ORIG 0x0858c4fa
}

#endif  // GAME_OS_API_H_
